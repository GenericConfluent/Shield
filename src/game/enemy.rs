use std::f32;

use super::*;
use bevy::prelude::*;

#[derive(Component, Default)]
pub struct Enemy {
    has_appeared: bool,
}

#[derive(Resource)]
pub struct EnemyAttributes {
    pub speed: f32,
}

impl Default for EnemyAttributes {
    fn default() -> Self {
        Self { speed: 80.0 }
    }
}

pub fn spawn_enemy(
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

            let margin = 50.0;

            let mut rng = rand::thread_rng();

            let edge = rng.gen_range(0..4);

            let (x, y) = match edge {
                0 => {
                    // Top edge
                    let x = rng.gen_range(area.min.x..area.max.x);
                    (x, area.max.y + margin)
                }
                1 => {
                    // Right edge
                    let y = rng.gen_range(area.min.y..area.max.y);
                    (area.max.x + margin, y)
                }
                2 => {
                    // Bottom edge
                    let x = rng.gen_range(area.min.x..area.max.x);
                    (x, area.min.y - margin)
                }
                _ => {
                    // Left edge
                    let y = rng.gen_range(area.min.y..area.max.y);
                    (area.min.x - margin, y)
                }
            };

            let player_pos = player.translation;
            let velocity =
                Vec2::from_array([player_pos.x - x, player_pos.y - y]).normalize_or_zero() * 80.;

            let mut sprite = Sprite::from_image(asset_server.load("enemy.png"));
            sprite.custom_size = Some(Vec2 { x: 32., y: 32. });

            // Figure out where to aim
            let mut enemy_transform = Transform::from_xyz(x, y, 1.);
            let angle = (enemy_transform.translation.xy() - player_pos.xy()).to_angle();
            enemy_transform.rotate_z(angle + f32::consts::PI);

            commands.spawn((
                EntityBundle {
                    sprite,
                    rigid_body: RigidBody::Kinematic,
                    transform: enemy_transform,
                    collider: ship_collider(),
                },
                CollisionLayers::new(GameLayer::Enemy, GameLayer::Default),
                LinearVelocity(velocity),
                Enemy::default(),
                StateScoped(GameState::Game),
            ));
        }
    }
}

pub fn despawn_enemy(
    mut commands: Commands,
    mut enemies: Query<(Entity, &Transform, &mut Enemy), Without<Camera>>,
    camera: Query<&OrthographicProjection, With<Camera>>,
) {
    let Ok(camera) = camera.get_single() else {
        warn_once!("Couldn't get camera for despawning enemies");
        return;
    };
    const BUFFER: f32 = 15.0;
    for (id, transform, mut enemy) in enemies.iter_mut() {
        if camera
            .area
            .inflate(BUFFER)
            .contains(transform.translation.xy())
        {
            enemy.has_appeared = true;
            continue;
        } else if enemy.has_appeared {
            // This enemy is certainly off screen
            commands.entity(id).despawn_recursive();
        }
    }
}
