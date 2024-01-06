# Generates rough estimates for neighbouring provinces, some pairs must be manually removed.

import json

# neighbours = {}
provinces = {}
neighbouring_pairs = []
blacklisted_pairs = []

class Province:
    def __init__(self, x, y, width, height):
        self.x = float(x)
        self.y = float(y)
        self.width = float(width)
        self.height = float(height)

    def overlaps(self, other):
        return not (self.x + self.width < other.x or
                    other.x + other.width < self.x or
                    self.y + self.height < other.y or
                    other.y + other.height < self.y)

with open("data/generated/province_dimensions.txt", "r") as province_mappings:
    for line in province_mappings.readlines():
        province_values = line.rstrip().split(',')
        province_id = province_values[0]
        provinces[province_id] = Province(*province_values[1:])

# for province in provinces:
#     neighbours[province] = []

for province in provinces:
    for other_province in provinces:
        if province == other_province:
            continue
        if provinces[province].overlaps(provinces[other_province]):
            # neighbours[province].append(other_province)
            if [other_province, province] not in neighbouring_pairs:
                if [other_province, province] not in blacklisted_pairs or [province, other_province] not in blacklisted_pairs:
                    neighbouring_pairs.append([province, other_province])

with open("data/generated/province_neighbours.txt", "w") as neighbour_file:
    for pair in neighbouring_pairs:
        neighbour_file.write(pair[0] + "," + pair[1] + "\n")

# Alternative storage method:
"""
with open("data/levels/a0.json", 'r') as level_file:
    province_data = json.load(level_file)
        
count = 0
for province_object in province_data["objects"]:
    province_data["objects"][count]["neighbours"] = neighbours[str(province_data["objects"][count]["id"])]
    count += 1

with open("data/levels/a0new.json", 'w') as level_output_file:
    json.dump(province_data, level_output_file, indent=4)
"""
