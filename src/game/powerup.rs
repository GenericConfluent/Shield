use super::*;
use bevy::prelude::*;

pub fn handle_slowmotion(
    mut commands: Commands,
    mut collect_events: EventReader<CollectItemEvent>,
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
        info!("Slowmotion Removed");
    } else if !*is_active && apply_effect {
        *is_active = true;
        info!("Slowmotion Applied");
    }
}

pub fn handle_speedboost(
    mut commands: Commands,
    mut collect_events: EventReader<CollectItemEvent>,
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
        info!("SpeedBoost Removed");
    } else if !*is_active && apply_effect {
        *is_active = true;
        info!("SpeedBoost Applied");
    }
}
