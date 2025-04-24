use super::{
    enemy::{Enemy, EnemyAttributes},
    player::PlayerAttributes,
    *,
};
use bevy::prelude::*;

pub fn handle_slowmotion(
    mut commands: Commands,
    mut collect_events: EventReader<CollectItemEvent>,
    enemy_attrs: Res<EnemyAttributes>,
    mut enemy_speeds: Query<&mut LinearVelocity, With<Enemy>>,
    time: Res<Time>,
    mut effect_time: Local<Timer>,
    mut is_active: Local<bool>,
) {
    effect_time.tick(time.delta());

    let mut apply_effect = false;

    for item in collect_events.read() {
        if item.item != PowerUp::TimeSlow {
            continue;
        }

        commands.entity(item.entity).despawn_recursive();

        let effect_duration = Duration::from_secs(7);
        *effect_time = Timer::new(effect_duration + effect_time.remaining(), TimerMode::Once);
        apply_effect = true;
    }

    if *is_active && effect_time.finished() {
        *is_active = false;
        for mut velocity in enemy_speeds.iter_mut() {
            velocity.0 = velocity.normalize_or_zero() * enemy_attrs.speed;
        }
    } else if !*is_active && apply_effect {
        *is_active = true;
        for mut velocity in enemy_speeds.iter_mut() {
            velocity.0 *= 0.5;
        }
    }
}

pub fn handle_speedboost(
    mut commands: Commands,
    mut collect_events: EventReader<CollectItemEvent>,
    mut player_attrs: ResMut<PlayerAttributes>,
    time: Res<Time>,
    mut effect_time: Local<Timer>,
    mut is_active: Local<bool>,
) {
    effect_time.tick(time.delta());

    let mut apply_effect = false;

    for item in collect_events.read() {
        if item.item != PowerUp::ShieldBoost {
            continue;
        }

        commands.entity(item.entity).despawn_recursive();

        let effect_duration = Duration::from_secs(7);
        *effect_time = Timer::new(effect_duration + effect_time.remaining(), TimerMode::Once);
        apply_effect = true;
    }

    if *is_active && effect_time.finished() {
        *is_active = false;
        player_attrs.speed = PlayerAttributes::default().speed;
    } else if !*is_active && apply_effect {
        *is_active = true;
        player_attrs.speed *= 1.5;
    }
}
