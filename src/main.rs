#![allow(clippy::complexity)]
use bevy::math::Vec3Swizzles;
use bevy::prelude::*;
use bevy_rapier2d::{prelude::*, rapier::prelude::CollisionEventFlags};
use rand::Rng;

fn main() {
    App::new()
        .add_plugins(DefaultPlugins)
        .add_plugins(game::GamePlugin)
        .add_plugins(menu::MenuPlugin)
        .add_state::<GameState>()
        .add_systems(Startup, setup)
        .add_systems(Update, resize_background)
        .run();
}

#[derive(Component)]
struct Background;

#[derive(Clone, Copy, Default, Eq, PartialEq, Debug, Hash, States)]
enum GameState {
    #[default]
    Menu,
    Game,
}

fn setup(mut commands: Commands) {
    commands.spawn(Camera2dBundle::default());
}

fn resize_background(
    mut background: Query<(&mut Transform, &Handle<Image>), With<Background>>,
    camera: Query<&OrthographicProjection, With<Camera>>,
    assets: Res<Assets<Image>>,
) {
    let visible_area = camera.single().area;
    let size = Vec2::new(
        visible_area.max.x - visible_area.min.x,
        visible_area.max.y - visible_area.min.y,
    );
    let mut background_iter = background.iter_mut();

    if let Some((mut transform, handle)) = background_iter.next() {
        if let Some(image) = assets.get(handle) {
            let background_size = image.size();
            let x_scale = size.x / background_size.x;
            let y_scale = size.y / background_size.y;

            let scale = if x_scale > y_scale { x_scale } else { y_scale };

            transform.scale.x = scale;
            transform.scale.y = scale;
        }

        if background_iter.next().is_some() {
            eprintln!("WARNING: There are multiple backgrounds spawned in currently");
        }
    }
}

mod menu {
    use super::*;

    pub struct MenuPlugin;

    impl Plugin for MenuPlugin {
        fn build(&self, app: &mut App) {
            app.add_systems(OnEnter(GameState::Menu), menu_setup);
            app.add_systems(Update, check_selection.run_if(in_state(GameState::Menu)));
        }
    }

    fn check_selection(
        keyboard_input: Res<Input<KeyCode>>,
        mut app_exit_events: ResMut<Events<bevy::app::AppExit>>,
        mut game_state: ResMut<NextState<GameState>>,
    ) {
        if keyboard_input.pressed(KeyCode::P) {
            game_state.set(GameState::Game);
            return;
        }
        if keyboard_input.pressed(KeyCode::H) {
            // Show help info
        }
        if keyboard_input.pressed(KeyCode::Q) {
            app_exit_events.send(bevy::app::AppExit);
        }
    }

    fn menu_setup(mut commands: Commands, asset_server: Res<AssetServer>) {
        commands
            .spawn(SpriteBundle {
                texture: asset_server.load("menuback.png"),
                transform: Transform::from_xyz(0., 0., 0.),
                ..default()
            })
            .insert(Background);

        commands
            .spawn(NodeBundle {
                style: Style {
                    width: Val::Percent(100.),
                    height: Val::Percent(100.),
                    justify_content: JustifyContent::Center,
                    flex_direction: FlexDirection::Column,
                    ..default()
                },
                ..default()
            })
            .with_children(|parent| {
                parent.spawn(
                    TextBundle::from_section(
                        "SHIELD",
                        TextStyle {
                            font: asset_server.load("fonts/SpaceAge.ttf"),
                            font_size: 50.,
                            color: Color::WHITE,
                        },
                    )
                    .with_text_alignment(TextAlignment::Center)
                    .with_style(Style {
                        position_type: PositionType::Relative,
                        top: Val::Px(40.),
                        ..default()
                    }),
                );
                parent
                    .spawn(NodeBundle {
                        style: Style {
                            display: Display::Flex,
                            flex_direction: FlexDirection::Column,
                            ..default()
                        },
                        ..default()
                    })
                    .with_children(|parent| {
                        for opt in ["Play(P)", "Help(H)", "Quit(Q)"] {
                            parent.spawn(TextBundle::from_section(
                                opt,
                                TextStyle {
                                    font: asset_server.load("fonts/EarthOrbiter.ttf"),
                                    font_size: 30.,
                                    color: Color::WHITE,
                                },
                            ));
                        }
                    });
            });
    }
}

mod game {
    use std::time::Duration;

    use super::*;

    pub struct GamePlugin;

    #[derive(Bundle)]
    struct EntityBundle {
        sprite: SpriteBundle,
        rigid_body: RigidBody,
        velocity: Velocity,
        collider: Collider,
        sleep: Sleeping,
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
        sprite: SpriteBundle,
        collider: Collider,
        sensor: Sensor,
    }

    impl Plugin for GamePlugin {
        fn build(&self, app: &mut App) {
            app.add_plugins(RapierPhysicsPlugin::<NoUserData>::pixels_per_meter(100.0))
                .add_plugins(RapierDebugRenderPlugin::default())
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
                        spawn_explosion,
                        handle_powerup,
                        animate_explosion,
                    )
                        .run_if(in_state(GameState::Game)),
                )
                // .add_systems(Last, check_end.run_if(in_state(GameState::Game)))
                .add_systems(OnExit(GameState::Game), game_cleanup);
        }
    }

    fn game_setup(
        mut commands: Commands,
        asset_server: Res<AssetServer>,
        mut rapier_config: ResMut<RapierConfiguration>,
        time: Res<Time>,
    ) {
        commands.insert_resource(EnemySpawnTimer(Timer::from_seconds(
            2.,
            TimerMode::Repeating,
        )));
        commands.insert_resource(PowerUpSpawnTimer(Timer::from_seconds(
            5.,
            TimerMode::Repeating,
        )));

        commands.insert_resource(RoundStarted(time.elapsed_seconds()));
        rapier_config.gravity = Vec2::ZERO;

        commands
            .spawn(SpriteBundle {
                texture: asset_server.load("gameback.png"),
                transform: Transform::from_xyz(0., 0., 0.),
                ..default()
            })
            .insert(Background);

        commands
            .spawn(NodeBundle {
                style: Style {
                    width: Val::Percent(100.),
                    height: Val::Percent(100.),
                    justify_content: JustifyContent::Center,
                    ..default()
                },
                ..default()
            })
            .with_children(|parent| {
                parent.spawn((
                    TextBundle::from_section(
                        format!("{}", 0.0),
                        TextStyle {
                            font: asset_server.load("fonts/SpaceMono.ttf"),
                            font_size: 50.,
                            color: Color::WHITE,
                        },
                    )
                    .with_text_alignment(TextAlignment::Center),
                    TimeDisplay,
                ));
            });

        commands
            .spawn(EntityBundle {
                sprite: SpriteBundle {
                    sprite: Sprite {
                        custom_size: Some(Vec2 { x: 20., y: 20. }),
                        ..default()
                    },
                    texture: asset_server.load("player.png"),
                    transform: Transform::from_xyz(0., 0., 1.),
                    ..default()
                },
                rigid_body: RigidBody::Dynamic,
                velocity: Velocity::zero(),
                collider: Collider::cuboid(20., 20.),
                sleep: Sleeping::disabled(),
            })
            .insert(Sensor)
            .insert(ActiveEvents::COLLISION_EVENTS)
            .insert(Damping {
                linear_damping: 2.,
                angular_damping: 20.,
            })
            .insert(ExternalForce::default())
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
                    let sprite = SpriteBundle {
                        sprite: Sprite {
                            custom_size: Some(Vec2::new(24., 24.)),
                            ..default()
                        },
                        texture: textures[i].clone(),
                        transform: Transform::from_xyz(x, y, 1.),
                        ..default()
                    };

                    let mut ent_commands = commands.spawn(CollectibleBundle {
                        sprite,
                        collider: Collider::ball(12.),
                        ..default()
                    });

                    if let Some(power_up) = power_ups.get(i) {
                        ent_commands.insert(*power_up);
                    } else {
                        ent_commands.insert(Bomb);
                        ent_commands.insert(ActiveEvents::COLLISION_EVENTS);
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
        display.single_mut().sections[0].value =
            format!("{:.2}", time.elapsed_seconds() - started.0);
    }

    trait AxisInput {
        fn axis(&self, pos: KeyCode, neg: KeyCode) -> f32;
    }

    impl AxisInput for Input<KeyCode> {
        fn axis(&self, pos: KeyCode, neg: KeyCode) -> f32 {
            self.pressed(pos) as i32 as f32 - self.pressed(neg) as i32 as f32
        }
    }

    fn player_control(
        mut player_impulse: Query<(&mut ExternalForce, &Transform), With<Player>>,
        keyboard_input: Res<Input<KeyCode>>,
    ) {
        if let Some((mut player_impulse, player_transform)) = player_impulse.iter_mut().next() {
            player_impulse.torque = keyboard_input.axis(KeyCode::A, KeyCode::D) / 3.;
            if keyboard_input.pressed(KeyCode::W) {
                player_impulse.force = player_transform.rotation.mul_vec3(Vec3::X).xy() * 80.;
            }
        }
    }

    // This could easily be applied to any other Texture atlas based anims.
    fn animate_explosion(
        mut commands: Commands,
        mut explosion_query: Query<
            (Entity, &mut TextureAtlasSprite, &mut ExplosionTimer),
            With<Explosion>,
        >,
        player: Query<Entity, With<Player>>,
        time: Res<Time>,
        mut game_state: ResMut<NextState<GameState>>,
    ) {
        const LAST: usize = 63; // TODO: Figure this out.
        for (entity, mut sprite, mut timer) in explosion_query.iter_mut() {
            if timer.0.tick(time.delta()).just_finished() {
                if sprite.index < LAST {
                    sprite.index += 1;
                    continue;
                } else if player.is_empty() {
                    game_state.set(GameState::Menu);
                }
                commands.entity(entity).despawn_recursive();
            }
        }
    }

    fn spawn_explosion(
        mut commands: Commands,
        mut explosion_event: EventReader<ExplosionEvent>,
        transforms: Query<(Entity, &Transform)>,
        asset_server: Res<AssetServer>,
        mut texture_atlases: ResMut<Assets<TextureAtlas>>,
    ) {
        for explosion in explosion_event.iter() {
            let texture = asset_server.load("explosion.png");
            let texture_atlas = texture_atlases.add(TextureAtlas::from_grid(
                texture,
                Vec2::new(512., 512.),
                8,
                8,
                None,
                None,
            ));

            // This hurts, not a little but alot.
            let position = transforms
                .iter()
                .find_map(|(ent, transform)| {
                    if explosion.0.iter().any(|involved| ent == *involved) {
                        Some(transform.translation)
                    } else {
                        None
                    }
                })
                .expect("Could not extract position of entity involved in explosion");

            let position_other = transforms
                .iter()
                .find_map(|(ent, transform)| {
                    if explosion.0.iter().any(|involved| ent == *involved) {
                        Some(transform.translation)
                    } else {
                        None
                    }
                })
                .expect("Could not extract position of entity involved in explosion");

            let mut avg_position = (position_other + position) / 2.;
            avg_position.z = 10.;

            commands
                .spawn(SpriteSheetBundle {
                    texture_atlas,
                    sprite: TextureAtlasSprite::new(3),
                    transform: Transform::from_translation(avg_position),
                    ..default()
                })
                .insert(ExplosionTimer(Timer::from_seconds(
                    0.02,
                    TimerMode::Repeating,
                )))
                .insert(Explosion);
            // Kill off the things involved in the explosion
            for ent in explosion.0.iter() {
                commands.entity(*ent).despawn_recursive();
            }
        }
    }

    fn handle_powerup(
        mut powerup_events: EventReader<PowerUpEvent>,
        mut enemy_speeds: Query<&mut Velocity, With<Enemy>>,
        mut effect_time_left: Local<Option<Timer>>,
    ) {
        const ENEMY_VELOCITY_MAG: f32 = 40.;

        enemy_speeds.for_each_mut(|mut velocity| {
            if let Some(timer) = effect_time_left.as_ref() {
                let multiplier = timer.finished() as i32 as f32;
                velocity.linvel = velocity.linvel.normalize() * ENEMY_VELOCITY_MAG * multiplier;
            }
        });

        const SLOWDOWN_TIME_S: f32 = 10.;
        for event in powerup_events.iter() {
            match event {
                _ => {
                    if let Some(timer) = effect_time_left.as_mut() {
                        timer.set_duration(
                            timer.duration() + Duration::from_secs_f32(SLOWDOWN_TIME_S),
                        );
                    } else {
                        *effect_time_left =
                            Some(Timer::from_seconds(SLOWDOWN_TIME_S, TimerMode::Once));
                    }
                }
            }
        }
    }

    fn check_collisions(
        mut collision_events: EventReader<CollisionEvent>,
        mut explosion_event: EventWriter<ExplosionEvent>,
        mut powerup_event: EventWriter<PowerUpEvent>,
        player: Query<Entity, With<Player>>,
        enemies: Query<Entity, (With<Enemy>, Without<Player>)>,
        bombs: Query<Entity, (With<Bomb>, Without<Enemy>, Without<Player>)>,
        power_ups: Query<(Entity, &PowerUp), (Without<Bomb>, Without<Enemy>, Without<Player>)>,
    ) {
        for collision in collision_events.iter() {
            dbg!(collision);
            if let CollisionEvent::Started(a, b, _flags) = collision {
                let ents = [a, b];
                if let Some(player) = player.iter().next() {
                    if let Some(player_idx) = ents.iter().position(|ent| **ent == player) {
                        let other_idx = 1 - player_idx;
                        let is_other = |ent: Entity| ent == *ents[other_idx];

                        if enemies.iter().any(is_other) || bombs.iter().any(is_other) {
                            explosion_event.send(ExplosionEvent(vec![*a, *b]));
                            continue;
                        }
                        if let Some((_, powerup)) = power_ups.iter().find(|(ent, _)| is_other(*ent))
                        {
                            powerup_event.send(PowerUpEvent(*powerup));
                            continue;
                        }
                    }
                }
                let enemy_in_collision = ents
                    .iter()
                    .any(|ent| enemies.iter().any(|enemy| **ent == enemy));
                let bomb_in_collision = ents
                    .iter()
                    .any(|ent| bombs.iter().any(|bomb| **ent == bomb));
                if enemy_in_collision && bomb_in_collision {
                    explosion_event.send(ExplosionEvent(vec![*a, *b]));
                }
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
                let velocity = Vec2::from_array([player_pos.x - x, player_pos.y - y])
                    .normalize_or_zero()
                    * 80.;

                commands
                    .spawn(EntityBundle {
                        sprite: SpriteBundle {
                            sprite: Sprite {
                                custom_size: Some(Vec2 { x: 20., y: 20. }),
                                ..default()
                            },
                            texture: asset_server.load("enemy.png"),
                            transform: Transform::from_xyz(x, y, 1.),
                            ..default()
                        },
                        rigid_body: RigidBody::Dynamic,
                        velocity: Velocity::linear(velocity),
                        collider: Collider::cuboid(20., 20.),
                        sleep: Sleeping::disabled(),
                    })
                    .insert(Enemy);
            }
        }
    }
}
