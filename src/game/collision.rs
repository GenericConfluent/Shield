use super::{enemy::Enemy, *};
use avian2d::prelude::*;
use bevy::prelude::*;

/// Given some contacts and an entity assumed to be a part of the collision this will
/// get the other. If the entity is not a part of the collision it will return none.
#[inline]
pub fn other_entity(contacts: &Contacts, first: Entity) -> Option<Entity> {
    if contacts.entity1 == first {
        Some(contacts.entity2)
    } else if contacts.entity2 == first {
        Some(contacts.entity1)
    } else {
        None
    }
}

pub fn item_player_collision(
    mut collision_event: EventReader<Collision>,
    mut collect_event: EventWriter<CollectItemEvent>,
    player: Query<Entity, With<Player>>,
    items: Query<&PowerUp, Without<Player>>,
) {
    let Ok(player) = player.get_single() else {
        return;
    };

    for Collision(contacts) in collision_event.read() {
        let Some(other) = other_entity(contacts, player) else {
            continue;
        };

        let Ok(item) = items.get(other) else {
            continue;
        };

        collect_event.send(CollectItemEvent {
            entity: other,
            item: *item,
        });
    }
}

pub fn enemy_player_collision(
    mut collision_event: EventReader<Collision>,
    mut commands: Commands,
    player: Query<Entity, With<Player>>,
    enemies: Query<&Enemy, Without<Player>>,
) {
    let Ok(player) = player.get_single() else {
        return;
    };

    for Collision(contacts) in collision_event.read() {
        let Some(other) = other_entity(contacts, player) else {
            continue;
        };

        if enemies.contains(other) {
            // enemy_player_collision.send(EnemyPlayerCollision(other));
            commands.trigger_targets(EnemyPlayerCollision(other), player);
        }
    }
}
