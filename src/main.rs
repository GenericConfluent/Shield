#![allow(clippy::complexity)]
use avian2d::prelude::*;
use bevy::math::Vec3Swizzles;
use bevy::prelude::*;
use rand::Rng;

mod game;
mod menu;

fn main() {
    App::new()
        .add_plugins(DefaultPlugins)
        .init_state::<GameState>()
        .add_plugins(game::GamePlugin)
        .add_plugins(menu::MenuPlugin)
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
    commands.spawn(Camera2d);
}

fn resize_background(
    mut background: Query<(&mut Transform, &Sprite), With<Background>>,
    camera: Query<&OrthographicProjection, With<Camera>>,
    assets: Res<Assets<Image>>,
) {
    let visible_area = camera.single().area;
    let size = Vec2::new(
        visible_area.max.x - visible_area.min.x,
        visible_area.max.y - visible_area.min.y,
    );
    let mut background_iter = background.iter_mut();

    if let Some((mut transform, sprite)) = background_iter.next() {
        if let Some(image) = assets.get(&sprite.image) {
            let background_size = image.size();
            let x_scale = size.x / background_size.x as f32;
            let y_scale = size.y / background_size.y as f32;

            let scale = x_scale.max(y_scale);

            transform.scale.x = scale;
            transform.scale.y = scale;
        }

        if background_iter.next().is_some() {
            eprintln!("WARNING: There are multiple backgrounds spawned in currently");
        }
    }
}
