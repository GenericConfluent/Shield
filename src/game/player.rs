use super::enemy::Enemy;
use super::spawn_explosion;
use super::EnemyPlayerCollision;
use super::EntityBundle;
use super::ExplosionTextureAtlas;
use super::GameLayer;
use super::GameState;
use super::PlayState;
use avian2d::prelude::*;
use bevy::prelude::*;

trait AxisInput {
    fn axis(&self, pos: KeyCode, neg: KeyCode) -> f32;
}

impl AxisInput for ButtonInput<KeyCode> {
    fn axis(&self, pos: KeyCode, neg: KeyCode) -> f32 {
        self.pressed(pos) as i32 as f32 - self.pressed(neg) as i32 as f32
    }
}

#[derive(Component)]
pub struct Player;

pub fn spawn_player(mut commands: Commands, asset_server: Res<AssetServer>) {
    let mut sprite = Sprite::from_image(asset_server.load("player.png"));
    sprite.custom_size = Some(Vec2::new(20.0, 20.0));

    commands.spawn((
        EntityBundle {
            sprite,
            rigid_body: RigidBody::Kinematic,
            transform: Transform::from_xyz(0., 0., 1.),
            collider: Collider::circle(20.),
        },
        CollisionLayers::new(GameLayer::Default, CollisionLayers::ALL_FILTERS.filters),
        Mass(2.0),
        LinearVelocity(Vec2::ZERO),
        MaxLinearSpeed(800.0),
        AngularVelocity(0.0),
        MaxAngularSpeed(200.0),
        TransformInterpolation,
        SleepingDisabled,
        Sensor,
        Player,
        StateScoped(GameState::Game),
    ));
}

// NOTE: This is supposed to be an observer.
pub fn player_death(
    _: Trigger<EnemyPlayerCollision>,
    mut commands: Commands,
    data: Res<ExplosionTextureAtlas>,
    enemies: Query<(Entity, &Transform), With<Enemy>>,
    mut play_state: ResMut<PlayState>,
) {
    if *play_state == PlayState::Playing {
        for (entity, transform) in enemies.iter() {
            spawn_explosion(&mut commands, &data, transform.translation);
            commands.entity(entity).despawn_recursive();
        }
        *play_state = PlayState::Died;
    }
}

pub fn player_control(
    mut player_query: Query<
        (&mut LinearVelocity, &mut AngularVelocity, &mut Transform),
        With<Player>,
    >,
    camera: Query<&OrthographicProjection, With<Camera>>,
    keyboard_input: Res<ButtonInput<KeyCode>>,
) {
    if let Ok((mut linear_velocity, mut angular_velocity, mut player_transform)) =
        player_query.get_single_mut()
    {
        // I may want to dampen
        // linear_velocity.0 = Vec2::ZERO;
        // angular_velocity.0 = 0.0;

        let rotation_speed = 3.0;
        if keyboard_input.pressed(KeyCode::KeyA) {
            angular_velocity.0 = rotation_speed;
        }
        if keyboard_input.pressed(KeyCode::KeyD) {
            angular_velocity.0 = -rotation_speed;
        }

        if keyboard_input.pressed(KeyCode::KeyW) {
            let direction = player_transform.rotation.mul_vec3(Vec3::X).xy();
            linear_velocity.0 = direction * 200.0; // Adjust speed as needed
        }

        let Ok(projection) = camera.get_single() else {
            warn_once!("Failed to get camera projection for wrap");
            return;
        };

        let bounds = projection.area;
        const BUFFER: f32 = 0.5;

        wrap_value(
            &mut player_transform.translation.x,
            bounds.min.x,
            bounds.max.x,
            BUFFER,
        );
        wrap_value(
            &mut player_transform.translation.y,
            bounds.min.y,
            bounds.max.y,
            BUFFER,
        );
    }
}

fn wrap_value(pos: &mut f32, min: f32, max: f32, buffer: f32) {
    let width = max - min;

    if *pos < min - buffer {
        *pos += width + buffer;
    } else if *pos > max + buffer {
        *pos -= width + buffer;
    }
}
