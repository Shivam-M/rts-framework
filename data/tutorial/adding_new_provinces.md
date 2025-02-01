1. Add a new area in `world_map.bmp` with a unique colour
2. Assign an ID by adding it to `province_colours.txt` in the format `id,red,green,blue`
3. Add and name the province in `levels/rts/0.json`
4. Assign the province to a nation in `levels/rts/1.json`
5. Run the game using the `loadMap()` method to calculate `generated/province_dimensions.txt`
6. Run `gen_neighbours.py` to esimate province neighbours in `generated/province_neighbours.txt`