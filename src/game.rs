use avian2d::prelude::*;
use std::time::Duration;

use super::*;

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

#[derive(Component)]
struct ExplosionTimer(Timer);

#[derive(Resource)]
struct RoundStarted(f32);

#[derive(Component)]
struct TimeDisplay;

#[derive(Component)]
struct Player;

#[derive(Component)]
struct Enemy;

#[derive(Component)]
struct Explosion;

#[derive(Event)]
struct PowerUpEvent(PowerUp);

#[derive(Event)]
struct ExplosionEvent(Vec<Entity>);

#[derive(Component, Clone, Copy)]
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

impl Plugin for GamePlugin {
    fn build(&self, app: &mut App) {
        app.add_plugins(
            PhysicsPlugins::default().set(PhysicsInterpolationPlugin::interpolate_all()),
        )
        .add_event::<ExplosionEvent>()
        .add_event::<PowerUpEvent>()
        .add_systems(OnEnter(GameState::Game), game_setup)
        .add_systems(
            Update,
            (
                spawn_enemy,
                check_collisions,
                player_control,
                display_time,
                spawn_items,
                // spawn_explosion,
                handle_powerup,
                animate_explosion,
            )
                .run_if(in_state(GameState::Game)),
        )
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

    commands.spawn((
        Sprite::from_image(asset_server.load("gameback.png")),
        Background,
    ));

    commands
        .spawn(Node {
            width: Val::Percent(100.),
            height: Val::Percent(100.),
            justify_content: JustifyContent::Center,
            ..default()
        })
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

    commands
        .spawn((
            EntityBundle {
                sprite: Sprite::from_image(asset_server.load("player.png")),
                rigid_body: RigidBody::Dynamic,
                transform: Transform::from_xyz(0., 0., 1.),
                collider: Collider::circle(20.),
            },
            CollisionLayers::new(GameLayer::Default, CollisionLayers::ALL_FILTERS.filters),
        ))
        .insert(Sensor)
        .insert(Player);
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
                sprite.custom_size = Some(Vec2::new(24., 24.));

                let mut ent_commands = commands.spawn((
                    CollectibleBundle {
                        sprite,
                        transform: Transform::from_xyz(x, y, 1.),
                        collider: Collider::circle(12.),
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
) {
    display.single_mut().0 = format!("{:.2}", time.elapsed_secs() - started.0);
}

trait AxisInput {
    fn axis(&self, pos: KeyCode, neg: KeyCode) -> f32;
}

impl AxisInput for ButtonInput<KeyCode> {
    fn axis(&self, pos: KeyCode, neg: KeyCode) -> f32 {
        self.pressed(pos) as i32 as f32 - self.pressed(neg) as i32 as f32
    }
}

fn player_control(
    mut commands: Commands,
    mut player_query: Query<(Entity, &Transform), With<Player>>,
    keyboard_input: Res<ButtonInput<KeyCode>>,
) {
    if let Some((player_entity, player_transform)) = player_query.iter_mut().next() {
        let mut player_commands = commands.entity(player_entity);
        if keyboard_input.any_pressed([KeyCode::KeyA, KeyCode::KeyD]) {
            let torque = keyboard_input.axis(KeyCode::KeyA, KeyCode::KeyD) / 3.;
            player_commands.insert(ExternalTorque::new(torque).with_persistence(false));
        }

        if keyboard_input.pressed(KeyCode::KeyW) {
            let force = player_transform.rotation.mul_vec3(Vec3::X).xy() * 80.;
            player_commands.insert(ExternalForce::new(force).with_persistence(false));
        }
    }
}

// This could easily be applied to any other Texture atlas based anims.
fn animate_explosion(
    mut commands: Commands,
    mut explosion_query: Query<(Entity, &mut Sprite, &mut ExplosionTimer), With<Explosion>>,
    player: Query<Entity, With<Player>>,
    time: Res<Time>,
    mut game_state: ResMut<NextState<GameState>>,
) {
    const LAST: usize = 63; // TODO: Figure this out.
    for (entity, mut sprite, mut timer) in explosion_query.iter_mut() {
        if timer.0.tick(time.delta()).just_finished() {
            // if sprite.texture_atlas.index < LAST {
            //     sprite.texture_atlas.index += 1;
            //     continue;
            // } else if player.is_empty() {
            //     game_state.set(GameState::Menu);
            // }
            // commands.entity(entity).despawn_recursive();
        }
    }
}

fn spawn_explosion(
    mut commands: Commands,
    mut explosion_event: EventReader<ExplosionEvent>,
    transforms: Query<(Entity, &Transform)>,
    asset_server: Res<AssetServer>,
    // mut texture_atlases: ResMut<Assets<TextureAtlas>>,
) {
    //     for explosion in explosion_event.iter() {
    //         let texture = asset_server.load("explosion.png");
    //         let texture_atlas = texture_atlases.add(TextureAtlas::from_grid(
    //             texture,
    //             Vec2::new(512., 512.),
    //             8,
    //             8,
    //             None,
    //             None,
    //         ));
    //
    //         // This hurts, not a little but alot.
    //         let position = transforms
    //             .iter()
    //             .find_map(|(ent, transform)| {
    //                 if explosion.0.iter().any(|involved| ent == *involved) {
    //                     Some(transform.translation)
    //                 } else {
    //                     None
    //                 }
    //             })
    //             .expect("Could not extract position of entity involved in explosion");
    //
    //         let position_other = transforms
    //             .iter()
    //             .find_map(|(ent, transform)| {
    //                 if explosion.0.iter().any(|involved| ent == *involved) {
    //                     Some(transform.translation)
    //                 } else {
    //                     None
    //                 }
    //             })
    //             .expect("Could not extract position of entity involved in explosion");
    //
    //         let mut avg_position = (position_other + position) / 2.;
    //         avg_position.z = 10.;
    //
    //         commands
    //             .spawn(SpriteSheetBundle {
    //                 texture_atlas,
    //                 sprite: TextureAtlasSprite::new(3),
    //                 transform: Transform::from_translation(avg_position),
    //                 ..default()
    //             })
    //             .insert(ExplosionTimer(Timer::from_seconds(
    //                 0.02,
    //                 TimerMode::Repeating,
    //             )))
    //             .insert(Explosion);
    //         // Kill off the things involved in the explosion
    //         for ent in explosion.0.iter() {
    //             commands.entity(*ent).despawn_recursive();
    //         }
    //     }
}

fn handle_powerup(
    mut powerup_events: EventReader<PowerUpEvent>,
    mut enemy_speeds: Query<&mut LinearVelocity, With<Enemy>>,
    mut effect_time_left: Local<Option<Timer>>,
) {
    const ENEMY_VELOCITY_MAG: f32 = 40.;

    for mut velocity in enemy_speeds.iter_mut() {
        if let Some(timer) = effect_time_left.as_ref() {
            let multiplier = timer.finished() as i32 as f32;
            velocity.0 = velocity.0.normalize() * ENEMY_VELOCITY_MAG * multiplier;
        }
    }

    const SLOWDOWN_TIME_S: f32 = 10.;
    for event in powerup_events.read() {
        match event {
            _ => {
                if let Some(timer) = effect_time_left.as_mut() {
                    timer.set_duration(timer.duration() + Duration::from_secs_f32(SLOWDOWN_TIME_S));
                } else {
                    *effect_time_left = Some(Timer::from_seconds(SLOWDOWN_TIME_S, TimerMode::Once));
                }
            }
        }
    }
}

fn check_collisions(
    mut collision_events: EventReader<Collision>,
    mut explosion_event: EventWriter<ExplosionEvent>,
    mut powerup_event: EventWriter<PowerUpEvent>,
    player: Query<Entity, With<Player>>,
    enemies: Query<Entity, (With<Enemy>, Without<Player>)>,
    bombs: Query<Entity, (With<Bomb>, Without<Enemy>, Without<Player>)>,
    power_ups: Query<(Entity, &PowerUp), (Without<Bomb>, Without<Enemy>, Without<Player>)>,
) {
    for Collision(contacts) in collision_events.read() {
        let ents = [contacts.entity1, contacts.entity2];
        if let Ok(player) = player.get_single() {
            if let Some(player_idx) = ents.iter().position(|ent| *ent == player) {
                let other_idx = 1 - player_idx;
                let is_other = |ent: Entity| ent == ents[other_idx];

                if enemies.iter().any(is_other) || bombs.iter().any(is_other) {
                    explosion_event.send(ExplosionEvent(vec![contacts.entity1, contacts.entity2]));
                    continue;
                }
                if let Some((_, powerup)) = power_ups.iter().find(|(ent, _)| is_other(*ent)) {
                    powerup_event.send(PowerUpEvent(*powerup));
                    continue;
                }
            }
        }
        let enemy_in_collision = ents
            .iter()
            .any(|ent| enemies.iter().any(|enemy| *ent == enemy));
        let bomb_in_collision = ents.iter().any(|ent| bombs.iter().any(|bomb| *ent == bomb));
        if enemy_in_collision && bomb_in_collision {
            explosion_event.send(ExplosionEvent(vec![contacts.entity1, contacts.entity2]));
        }
    }
}

fn spawn_enemy(
    mut commands: Commands,
    asset_server: Res<AssetServer>,
    mut spawn_timer: ResMut<EnemySpawnTimer>,
    camera: Query<&OrthographicProjection, With<Camera>>,
    player: Query<&Transform, With<Player>>,
    time: Res<Time>,
) {
    if let Some(player) = player.iter().next() {
        if spawn_timer.0.tick(time.delta()).finished() {
            let area = camera.single().area;

            let mut rng = rand::thread_rng();
            let x: f32 = rng.gen_range(area.min.x..area.max.x);
            let y: f32 = rng.gen_range(area.min.y..area.max.y);

            let player_pos = player.translation;
            let velocity =
                Vec2::from_array([player_pos.x - x, player_pos.y - y]).normalize_or_zero() * 80.;

            let mut sprite = Sprite::from_image(asset_server.load("enemy.png"));
            sprite.custom_size = Some(Vec2 { x: 20., y: 20. });

            commands
                .spawn(EntityBundle {
                    sprite,
                    rigid_body: RigidBody::Dynamic,
                    transform: Transform::from_xyz(x, y, 1.),
                    collider: Collider::circle(20.),
                })
                .insert(Enemy);
        }
    }
}
