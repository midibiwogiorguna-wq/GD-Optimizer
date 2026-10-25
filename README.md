# GD-Optimizer

GD-Optimizer is an Android-focused rendering optimization mod for Geometry Dash 2.2081, built with Geode.

## Current features (v1.1.0)

- **Potato Mode:** skips drawing particle systems to reduce particle rendering work.
- **Reduce Particles:** skips drawing particle systems that currently have 12 or more active particles.
- **FPS monitor:** displays average FPS and frame time while playing.

## Important limitation

The particle options skip drawing; they do not stop particle simulation, and they do not guarantee higher FPS. Some effects may disappear when these options are enabled. Test them in a practice run and disable them if an effect becomes hard to read. Performance depends on the device, level, and game's own frame pacing.

Builds are produced for Android 64-bit through GitHub Actions.
