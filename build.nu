const game_name = "shield"

const itch_username = "genericconfluent"
const itch_url = "itch.io"

const web_dir = "target/web-build"
const assets_dir = "assets"
const template_file = "template.html"

def has_dependency [dep: string, required: bool = true]: nothing -> bool {
    if (which $dep | is-empty) {
        if $required {
            print $"ERROR: missing required ($dep)"
            exit 1
        } else {
            print $"WARN: missing ($dep)"
        }
        true
    }
    false
}

has_dependency "cargo"
has_dependency "wasm-bindgen"
let extra_opt = (has_dependency "wasm-opt" false)
let can_zip = (has_dependency "7z" false)
let can_upload = (has_dependency "butler" false)

# Generate WASM
cargo build --release --target wasm32-unknown-unknown

# Assemble Web Directory
# NOTE: We are in nushell, parents are automatically created, -p is incompatible
mkdir web_dir

wasm-bindgen --out-dir $web_dir --target web --no-typescript $"target/wasm32-unknown-unknown/release/($game_name).wasm"
if $extra_opt {
    wasm-opt -O3 -o output.wasm input.wasm
}

if ($assets_dir | path exists) {
    cp -r $assets_dir $web_dir 
} else {
    print "WARN: could not copy assets, directory not found"
}

if ($template_file | path exists) {
    let content = (open $template_file | str replace "{{GAME_NAME}}" $game_name)
    $content | save -f $"($web_dir)/index.html"
} else {
    # May work without assets, but will definetly need an index file
    print "ERROR: could not find template"
    exit 1
}

# Zip and Deploy
if not $can_zip {
    print "INFO: Skipping zip"
    exit 0
}

let timestamp = (date now | format date "%Y%m%d_%H%M%S")
let archive_file = $"target/($game_name)-web-($timestamp).zip"

let project_root = (pwd)
cd $web_dir
7z a -tzip $"($project_root)/($archive_file)" ./*
cd $project_root

if not $can_upload {
    print "INFO: Skipping upload"
    exit 0
}

butler push $archive_file $"$(itch_username)/$(itch_game_url):web" --userversion $timestamp
