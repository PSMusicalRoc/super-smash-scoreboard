# Super Smash Scoreboard

## Overview

Super Smash Scoreboard is a program developed to help Smash Bros. streamers change images and text displayed on a stream overlay more automatically.

## The Problem

Changing the text for players' names, images for the characters, and other stream essentials can be time consuming sometimes. There's not a standard way of changing these things other than doing it manually through the streaming software (I'm going to specifically mention <a href="https://obsproject.com/">OBS</a>, and use it as my example from here on out).

I saw this problem and decided "Well, I've got nothing better to do today... let's make a program!"

## My Solution

My solution is Super Smash Scoreboard, which as of this writing (being **ROCINSERTDATEHERE**) looks something like this:

![](readmeImages/screenshot.png)

Super Smash Scoreboard is a C++ program using the <a>Dear ImGui library</a> with an <a href="https://www.libsdl.org/">SDL2</a> (plus <a href="https://www.libsdl.org/projects/SDL_image/">SDL2_image</a>) and <a href="https://www.opengl.org/">OpenGL 4.6 backend</a>. Right now it's being developed for Windows, but since all the dependencies are cross-platform or base C++ libraries, it should be able to compile for Linux and Mac OS in the future.

Currently the featureset looks something like this:

- okay then
- something else
- one more thing

## Installation

1. Go to the <a href="https://github.com/PSMusicalRoc/SmashScoreboard/releases/latest">releases page</a> of the GitHub repository and download the release (the .zip file that isn't source code).

2. Go to the .zip file in your files and extract it.

3. Profit.

It really is that simple. The executable is called "SmashScoreboard.exe" and should load the program when opened, provided that you have OpenGL 4.6 or later (as of right now, 4.6 is the latest version.)

## Using Smash Scoreboard

