import matplotlib.pyplot as plt
import matplotlib.patches as patches

# Defining the blocks with their dimensions and positions
blocks = {}

data_in = open("sample.in", "r")
line_in = data_in.readlines()
data_out = open("sample.out", "r")
line_out = data_out.readlines()

for i in range(len(line_in) - 1):
    list_1 = line_in[i+1].split()
    list_2 = line_out[i+2].split()
    
    if len(list_2) == 4:
        blocks[list_1[0]] = {"length": int(list_1[1]), "width": int(list_1[2]), "x": int(list_2[1]), "y": int(list_2[2])}
    else:
        blocks[list_1[0]] = {"length": int(list_1[2]), "width": int(list_1[1]), "x": int(list_2[1]), "y": int(list_2[2])}
        
# Setting up the plot
fig, ax = plt.subplots()
ax.set_title('Block Positions')
ax.set_xlabel('X coordinate')
ax.set_ylabel('Y coordinate')

# Adding each block as a rectangle to the plot
for block_id, block in blocks.items():
    rect = patches.Rectangle(
        (block["x"], block["y"]), block["width"], block["length"], 
        linewidth=1, edgecolor='black', facecolor='none', label=block_id
    )
    ax.add_patch(rect)
    # Adding the block ID text inside the rectangle
    plt.text(block["x"] + block["width"]/2, block["y"] + block["length"]/2, block_id, 
             ha='center', va='center', color='blue')

# Adjusting the plot limits and showing the plot
ax.set_xlim(0, max(block["x"] + block["width"] for block in blocks.values()) + 10)
ax.set_ylim(0, max(block["y"] + block["length"] for block in blocks.values()) + 10)
plt.grid(True)
plt.savefig("check.jpg")
