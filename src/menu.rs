use crate::{Background, GameState};
use bevy::{app::AppExit, prelude::*};

pub struct MenuPlugin;

impl Plugin for MenuPlugin {
    fn build(&self, app: &mut App) {
        app.add_systems(OnEnter(GameState::Menu), menu_setup);
        app.add_systems(Update, check_selection.run_if(in_state(GameState::Menu)));
    }
}

fn check_selection(
    keyboard_input: Res<ButtonInput<KeyCode>>,
    mut app_exit_events: ResMut<Events<AppExit>>,
    mut game_state: ResMut<NextState<GameState>>,
) {
    if keyboard_input.pressed(KeyCode::KeyP) {
        game_state.set(GameState::Game);
        return;
    }
    if keyboard_input.pressed(KeyCode::KeyH) {
        // Show help info
    }
    if keyboard_input.pressed(KeyCode::KeyQ) {
        app_exit_events.send(AppExit::Success);
    }
}

fn menu_setup(mut commands: Commands, asset_server: Res<AssetServer>) {
    commands
        .spawn(Sprite::from_image(asset_server.load("menuback.png")))
        .insert(Background);

    commands
        .spawn(Node {
            width: Val::Percent(100.),
            height: Val::Percent(100.),
            justify_content: JustifyContent::Center,
            flex_direction: FlexDirection::Column,
            ..default()
        })
        .with_children(|parent| {
            parent.spawn((
                Text::new("SHIELD"),
                TextFont {
                    font: asset_server.load("fonts/SpaceAge.ttf"),
                    font_size: 50.,
                    ..default()
                },
                TextColor(Color::WHITE),
                TextLayout::new_with_justify(JustifyText::Center),
                Node {
                    position_type: PositionType::Relative,
                    top: Val::Px(40.),
                    ..default()
                },
            ));

            parent
                .spawn(Node {
                    display: Display::Flex,
                    flex_direction: FlexDirection::Column,
                    ..default()
                })
                .with_children(|parent| {
                    for opt in ["Play(P)", "Help(H)", "Quit(Q)"] {
                        parent.spawn((
                            Text::new(opt),
                            TextFont {
                                font: asset_server.load("fonts/EarthOrbiter.ttf"),
                                font_size: 30.,
                                ..default()
                            },
                            TextColor(Color::WHITE),
                        ));
                    }
                });
        });
}
