# [annoy]

`[annoy]` is a Max wrapper around [Annoy](https://github.com/spotify/annoy),
the Approximate Nearest Neighbors search index.


## Developing

### Installing dependencies

#### Max SDK

[Download the Max SDK](https://github.com/Cycling74/max-sdk).

In Max, make sure to include the SDK's `externals` directory in
**Options** > **File Preferences...**

#### Annoy

[Clone annoy](https://github.com/spotify/annoy).

In Xcode, make sure that **Header Search Paths** includes `annoy/src`.

### Building

Build from the Xcode project.
