1. Add a new area in `world_map.bmp` with a unique colour
2. Assign an ID by appending it to `province_colours.txt` in the format `id,red,green,blue`
3. Add and name the province in `levels/rts/00-provinces.json`
4. Assign the province to a nation in `levels/rts/02-nations.json`
5. Run the game once using the `loadMap(...)` method to calculate `generated/province_dimensions.txt` and generate the province images
6. Run `gen_neighbours.py` to estimate province neighbours in `generated/province_neighbours.txt`