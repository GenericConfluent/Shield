# History
I originally wrote this project near the end of seventh grade by following along with a [game
engine tutorial](https://www.youtube.com/watch?v=tgXEws1Ef8s&list=PLRtjMdoYXLf4md1jeCkdPJYpDgPkjT9Di&pp=iAQB)
and then skimming through the SFML API docs to figure out how to do what I wanted to do.
I wanted to revisit it a couple times in the past couple years, but I originally setup the project
on macos and wrote it in Xcode, while not checking my project configs into git. Recently I've needed
to refresh my C++ for school so I [found a good `CMakeLists.txt`](https://github.com/SFML/cmake-sfml-project/blob/master/CMakeLists.txt) 
use, and started the cleanup. 

&mdash; Generic

# Building
Note: remember you need cmake installed. First you will of course need to clone the repo:
```bash
git clone https://github.com/GenericConfluent/Shield.git
```

Here if you are on windows you may need to check out the original `CMakeLists.txt` I used to get
the project building, since I only included what was needed to get it working on my Linux environment.
Next cd into the project directory and run:
```bash
cmake .
make
```

Then while making sure you are in the project root directory you can run:
```
bin/shield
```
You need to be in the projet root since the binary attempts to load paths 
relative to the pwd. (i.e. it looks for `assets/...` when trying to load
textures, fonts, and audio).
