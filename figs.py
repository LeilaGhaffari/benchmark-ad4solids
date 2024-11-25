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

# Plot computational graphs
def plot_computational_graph():
    # Forward Mode Graph
    forward_graph = nx.DiGraph()
    forward_graph.add_edges_from([
        ("x", "x^2"),
        ("x^2", "x^2 * y"),
        ("y", "x^2 * y"),
        ("x", "sin(x)"),
        ("sin(x)", "f"),
        ("x^2 * y", "f"),
    ])

    pos = {
        "f": (1, 2),
        "sin(x)": (.5, 1),
        "x^2 * y": (1.5, 1),
        "x^2": (1.25, 0),
        "x": (0.25, -1),
        "y": (2.25, -1),
    }

    # Draw nodes and edges
    nx.draw(
        forward_graph, pos, with_labels=True, node_color='skyblue', edge_color='black',
        node_size=3000, font_size=10, font_weight='bold', arrowsize=20
    )

    nx.draw_networkx_edge_labels(
        forward_graph, pos,
        edge_labels={
            ("x", "x^2"): "2x",
            ("x^2", "x^2 * y"): "y",
            ("y", "x^2 * y"): "x^2",
            ("x", "sin(x)"): "cos(x)",
            ("sin(x)", "f"): "1",
            ("x^2 * y", "f"): "1",
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
        ("f", "sin(x)"),
        ("f", "x^2 * y"),
        ("sin(x)", "x"),
        ("x^2 * y", "x^2"),
        ("x^2 * y", "y"),
        ("x^2", "x"),
    ])


    nx.draw(
        reverse_graph, pos, with_labels=True, node_color='lightgreen', edge_color='black',
        node_size=3000, font_size=10, font_weight='bold', arrowsize=20
    )

    nx.draw_networkx_edge_labels(
        reverse_graph, pos,
        edge_labels={
            ("f", "sin(x)"): "1",
            ("f", "x^2 * y"): "1",
            ("sin(x)", "x"): "cos(x)",
            ("x^2 * y", "x^2"): "y",
            ("x^2 * y", "y"): "x^2",
            ("x^2", "x"): "2x",
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
