mod collision;
mod enemy;
mod player;
mod powerup;

use super::*;
use avian2d::{math::Vector, prelude::*};
use collision::{enemy_player_collision, item_player_collision};
use enemy::{despawn_enemy, spawn_enemy};
use player::{player_control, player_death, Player};
use powerup::{handle_slowmotion, handle_speedboost};
use std::time::Duration;

pub struct GamePlugin;

#[derive(Bundle)]
struct EntityBundle {
    sprite: Sprite,
    rigid_body: RigidBody,
    transform: Transform,
    collider: Collider,
}

#[derive(Resource)]
struct EnemySpawnTimer(Timer);

#[derive(Resource)]
struct PowerUpSpawnTimer(Timer);

#[derive(Component, Deref, DerefMut)]
struct ExplosionTimer(Timer);

#[derive(Resource)]
struct RoundStarted(f32);

#[derive(Component)]
struct TimeDisplay;

#[derive(Component)]
struct Explosion;

#[derive(Component)]
pub struct AnimationIndices {
    first: usize,
    last: usize,
}

#[derive(Event, Debug)]
struct CollectItemEvent {
    entity: Entity,
    item: PowerUp,
}

#[derive(Resource)]
pub struct ExplosionTextureAtlas {
    texture: Handle<Image>,
    atlas: Handle<TextureAtlasLayout>,
}

impl FromWorld for ExplosionTextureAtlas {
    fn from_world(world: &mut World) -> Self {
        let texture = world.load_asset("explosion.png");
        let atlas = world.add_asset(TextureAtlasLayout::from_grid(
            UVec2::new(512, 512),
            8,
            8,
            None,
            None,
        ));
        ExplosionTextureAtlas { texture, atlas }
    }
}

#[derive(Event)]
struct ExplosionEvent(Vec<Entity>);

#[derive(Event)]
struct EnemyPlayerCollision(Entity);

#[derive(Resource, PartialEq)]
pub enum PlayState {
    Playing,
    Died,
    ReadyForMenu,
}

#[derive(Component, Clone, Copy, Debug, PartialEq)]
enum PowerUp {
    TimeSlow,
    ShieldBoost,
    // Could also do a lazer beam
}

#[derive(Component)]
struct Bomb;

#[derive(Bundle, Default)]
struct CollectibleBundle {
    sprite: Sprite,
    transform: Transform,
    collider: Collider,
    sensor: Sensor,
}

#[derive(PhysicsLayer, Default)]
enum GameLayer {
    #[default]
    Default,
    Collectible,
    Enemy,
}

pub fn ship_collider() -> Collider {
    Collider::triangle(
        Vec2::new(-16.0, 16.0),
        Vec2::new(-16.0, -16.0),
        Vec2::new(16.0, 0.0),
    )
}

impl Plugin for GamePlugin {
    fn build(&self, app: &mut App) {
        app.add_plugins(
            PhysicsPlugins::default()
                .with_length_unit(100.0)
                .set(PhysicsInterpolationPlugin::interpolate_all()),
        )
        .add_plugins(PhysicsDebugPlugin::default())
        .insert_resource(Gravity(Vec2::ZERO))
        .insert_resource(PlayState::Playing)
        .add_event::<ExplosionEvent>()
        .add_event::<EnemyPlayerCollision>()
        .add_event::<CollectItemEvent>()
        .add_systems(OnEnter(GameState::Game), (game_setup, player::spawn_player))
        .add_systems(
            PreUpdate,
            (item_player_collision, enemy_player_collision).run_if(in_state(GameState::Game)),
        )
        .add_systems(
            Update,
            (
                // Enemy
                spawn_enemy,
                despawn_enemy.after(spawn_enemy),
                // Items
                spawn_items,
                handle_slowmotion,
                handle_speedboost,
                perform_explosion,
                // Player
                // player::player_control,
                // UI
                display_time,
                exit_game_state,
            )
                .run_if(in_state(GameState::Game)),
        )
        .add_systems(FixedUpdate, player_control)
        // .add_systems(Last, check_end.run_if(in_state(GameState::Game)))
        .add_systems(OnExit(GameState::Game), game_cleanup);
    }
}

fn game_setup(mut commands: Commands, asset_server: Res<AssetServer>, time: Res<Time>) {
    commands.insert_resource(EnemySpawnTimer(Timer::from_seconds(
        2.,
        TimerMode::Repeating,
    )));
    commands.insert_resource(PowerUpSpawnTimer(Timer::from_seconds(
        5.,
        TimerMode::Repeating,
    )));

    commands.insert_resource(RoundStarted(time.elapsed_secs()));
    commands.insert_resource(PlayState::Playing);
    commands.init_resource::<ExplosionTextureAtlas>();

    // commands.spawn((
    //     Sprite::from_image(asset_server.load("gameback.png")),
    //     Background,
    // ));

    commands
        .spawn((
            Node {
                width: Val::Percent(100.),
                height: Val::Percent(100.),
                justify_content: JustifyContent::Center,
                ..default()
            },
            StateScoped(GameState::Game),
        ))
        .with_children(|parent| {
            parent.spawn((
                Text::new(format!("{}", 0.0)),
                TextFont {
                    font: asset_server.load("fonts/SpaceMono.ttf"),
                    font_size: 50.,
                    ..default()
                },
                TextColor(Color::WHITE),
                TextLayout::new_with_justify(JustifyText::Center),
                TimeDisplay,
            ));
        });
}

fn game_cleanup(
    mut commands: Commands,
    to_despawn: Query<Entity, Or<(With<Collider>, With<TimeDisplay>, With<Background>)>>,
) {
    for ent in &to_despawn {
        commands.entity(ent).despawn_recursive();
    }

    commands.remove_resource::<EnemySpawnTimer>();
    commands.remove_resource::<PowerUpSpawnTimer>();
    commands.remove_resource::<RoundStarted>();
}

pub fn exit_game_state(
    explosions: Query<Entity, With<Explosion>>,
    play_state: Res<PlayState>,
    mut next_state: ResMut<NextState<GameState>>,
) {
    if *play_state == PlayState::Died && explosions.is_empty() {
        next_state.set(GameState::Menu);
    }
}

fn spawn_items(
    mut commands: Commands,
    current_items: Query<&Collider, Or<(With<Bomb>, With<PowerUp>)>>,
    camera: Query<&OrthographicProjection, With<Camera>>,
    mut spawn_timer: ResMut<PowerUpSpawnTimer>,
    asset_server: Res<AssetServer>,
    time: Res<Time>,
) {
    let spawn_probs: [f32; 3] = [1. / 3.; 3];
    let textures: [_; 3] = [
        asset_server.load("timeslow.png"),
        asset_server.load("shieldboost.png"),
        asset_server.load("bomb.png"),
    ];
    let power_ups = [PowerUp::TimeSlow, PowerUp::ShieldBoost];

    if spawn_timer.0.tick(time.delta()).finished() && current_items.iter().count() < 5 {
        let area = camera.single().area;

        let mut rng = rand::thread_rng();
        let choice = rng.gen::<f32>();

        let x: f32 = rng.gen_range(area.min.x..area.max.x);
        let y: f32 = rng.gen_range(area.min.y..area.max.y);

        let mut acc = 0.;
        for (i, prob) in spawn_probs.iter().enumerate() {
            acc += prob;
            if choice < acc {
                let mut sprite = Sprite::from_image(textures[i].clone());
                sprite.custom_size = Some(Vec2::new(32., 32.));

                let mut ent_commands = commands.spawn((
                    CollectibleBundle {
                        sprite,
                        transform: Transform::from_xyz(x, y, 1.),
                        collider: Collider::circle(16.),
                        ..default()
                    },
                    CollisionLayers::new(GameLayer::Collectible, GameLayer::Default),
                ));

                if let Some(power_up) = power_ups.get(i) {
                    ent_commands.insert(*power_up);
                } else {
                    ent_commands.insert(Bomb);
                }

                break;
            }
        }
    }
}

fn display_time(
    mut display: Query<&mut Text, With<TimeDisplay>>,
    started: Res<RoundStarted>,
    time: Res<Time>,
    play_state: Res<PlayState>,
) {
    if *play_state == PlayState::Playing {
        display.single_mut().0 = format!("{:.2}", time.elapsed_secs() - started.0);
    }
}

fn perform_explosion(
    mut commands: Commands,
    mut active_explosions: Query<
        (Entity, &AnimationIndices, &mut Sprite, &mut ExplosionTimer),
        With<Explosion>,
    >,
    time: Res<Time>,
) {
    for (entity, indices, mut sprite, mut timer) in active_explosions.iter_mut() {
        timer.tick(time.delta());
        if timer.just_finished() {
            if let Some(atlas) = &mut sprite.texture_atlas {
                if atlas.index == indices.last {
                    commands.entity(entity).despawn_recursive();
                } else {
                    atlas.index += 1;
                }
            }
        }
    }
}

pub fn spawn_explosion(commands: &mut Commands, data: &ExplosionTextureAtlas, target: Vec3) {
    commands.spawn((
        Sprite::from_atlas_image(
            data.texture.clone(),
            TextureAtlas {
                layout: data.atlas.clone(),
                index: 0,
            },
        ),
        Transform {
            translation: target.with_z(10.0),
            ..default()
        },
        ExplosionTimer(Timer::from_seconds(0.02, TimerMode::Repeating)),
        AnimationIndices { first: 0, last: 63 },
        Explosion,
        StateScoped(GameState::Game),
    ));
}
