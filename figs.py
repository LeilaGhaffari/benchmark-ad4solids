import numpy as np
import matplotlib.pyplot as plt
import networkx as nx

# Function definition
def f(x, y):
    return np.sin(x) + x**2 * y

# Gradient computation for forward mode
def forward_mode(x, y):
    dz1_dx = np.cos(x)          # Derivative of z1 = sin(x)
    dz2_dx = 2 * x * y          # Derivative of z2 = x^2 * y
    dz2_dy = x**2               # Derivative of z2 = x^2 * y
    df_dx = dz1_dx + dz2_dx     # Chain rule for df/dx
    df_dy = dz2_dy              # Chain rule for df/dy
    return df_dx, df_dy

# Gradient computation for reverse mode
def reverse_mode(x, y):
    z1 = np.sin(x)
    z2 = x**2 * y

    dz1_dx = np.cos(x)          # Derivative of z1 = sin(x)
    dz2_dx = 2 * x * y          # Derivative of z2 = x^2 * y
    dz2_dy = x**2               # Derivative of z2 = x^2 * y

    df_dz1 = 1                  # Sensitivity of f to z1
    df_dz2 = 1                  # Sensitivity of f to z2

    df_dx = df_dz1 * dz1_dx + df_dz2 * dz2_dx
    df_dy = df_dz2 * dz2_dy
    return df_dx, df_dy

def plot_computational_graph():
    # Forward Mode Graph
    forward_graph = nx.DiGraph()
    forward_graph.add_edges_from([
        ("x", "pow2"),
        ("pow2", "*"),
        ("y", "*"),
        ("x", "sin"),
        ("sin", "+"),
        ("*", "+"),
        ("+", "f"),
    ])

    pos = {
        "f": (1, 8.5),
        "+": (1, 6),
        "sin": (.5, 1),
        "*": (1.5, 1),
        "pow2": (1, 0),
        "x": (0.25, -1),
        "y": (2, -1),
    }

    # Define node colors
    node_colors = []
    for node in forward_graph.nodes:
        if node == "f":
            node_colors.append("lightgreen")
        elif node == "x" or node == "y":
            node_colors.append("orange")
        else:
            node_colors.append("skyblue")

    # Draw nodes and edges for Forward Mode
    nx.draw(
        forward_graph, pos, with_labels=True, node_color=node_colors, edge_color='black',
        node_size=3000, font_size=10, font_weight='bold', arrowsize=20
    )

    nx.draw_networkx_edge_labels(
        forward_graph, pos,
        edge_labels={
            ("x", "pow2"): "$\dot{x}$",
            ("pow2", "*"): "2x $\dot{x}$",
            ("y", "*"): "$\dot{y}$",
            ("x", "sin"): "$\dot{x}$",
            ("sin", "+"): "cos(x) $\dot{x}$",
            ("*", "+"): "2x$\dot{x}$y + pow2(x) $\dot{y}$",
        },
        font_color='red'
    )
    plt.title("Forward Mode Computational Graph (Flipped)", fontsize=14)
    plt.tight_layout()
    plt.savefig("forward_mode_graph.png")
    plt.close()

    # Reverse Mode Graph
    reverse_graph = nx.DiGraph()
    reverse_graph.add_edges_from([
        ("f", "+"),
        ("+", "sin"),
        ("+", "*"),
        ("sin", "x"),
        ("*", "pow2"),
        ("*", "y"),
        ("pow2", "x"),
    ])

        # Define node colors for Reverse Mode
    node_colors_reverse = []
    for node in reverse_graph.nodes:
        if node == "f":
            node_colors_reverse.append("orange")
        elif node == "x" or node == "y":
            node_colors_reverse.append("lightgreen")
        else:
            node_colors_reverse.append("skyblue")

    nx.draw(
        reverse_graph, pos, with_labels=True, node_color=node_colors_reverse, edge_color='black',
        node_size=3000, font_size=10, font_weight='bold', arrowsize=20
    )

    nx.draw_networkx_edge_labels(
        reverse_graph, pos,
        edge_labels={
            ("+", "sin"): "1",
            ("+", "*"): "1",
            ("sin", "x"): "cos(x)",
            ("*", "pow2"): "y",
            ("*", "y"): "pow2",
            ("pow2", "x"): "2x",
        },
        font_color='red'
    )
    plt.title("Reverse Mode Computational Graph (Flipped)", fontsize=14)
    plt.tight_layout()
    plt.savefig("reverse_mode_graph.png")
    plt.close()


# Run computations and plot graphs
if __name__ == "__main__":
    x, y = 2.0, 3.0

    # Compute gradients
    fx, fy = forward_mode(x, y)
    print(f"Forward Mode Gradients: df/dx = {fx}, df/dy = {fy}")

    rx, ry = reverse_mode(x, y)
    print(f"Reverse Mode Gradients: df/dx = {rx}, df/dy = {ry}")

    # Plot computational graphs
    plot_computational_graph()
    print("Computational graphs saved as 'forward_mode_graph.png' and 'reverse_mode_graph.png'.")
