#!/usr/bin/env python3
# Makes the figures of the report from results/experiments/*.csv.
#
# Usage (from anywhere): python3 make_plots.py
# Output: results/plots/*.png
#
# Colours follow the method (same colour in every figure). Exact Search is the
# neutral reference (dark gray). Every method also has its own marker shape
# and a direct label, so colour is never the only way to tell them apart.

import csv
import os

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

HERE = os.path.dirname(os.path.abspath(__file__))
RESULTS = os.path.join(HERE, "results", "experiments")
PLOTS = os.path.join(HERE, "results", "plots")

SURFACE = "#fcfcfb"
TEXT = "#0b0b0b"
TEXT_2 = "#52514e"
GRID = "#e4e3df"

# method -> (label, colour, marker)
STYLE = {
    "exact": ("Exact", "#52514e", "s"),
    "ivfflat": ("IVFFlat", "#2a78d6", "o"),
    "lsh": ("LSH", "#eb6834", "^"),
    "hypercube": ("Hypercube", "#1baf7a", "D"),
}
ORDER = ["exact", "ivfflat", "lsh", "hypercube"]
DS = [1000, 5000, 10000, 25000]


def read_csv(name):
    with open(os.path.join(RESULTS, name)) as f:
        return list(csv.DictReader(f))


def style_axes(ax, title, xlabel, ylabel):
    ax.set_facecolor(SURFACE)
    ax.set_title(title, loc="left", color=TEXT, fontsize=12, pad=12)
    ax.set_xlabel(xlabel, color=TEXT_2)
    ax.set_ylabel(ylabel, color=TEXT_2)
    ax.grid(True, color=GRID, linewidth=0.8)
    ax.set_axisbelow(True)
    for side in ("top", "right"):
        ax.spines[side].set_visible(False)
    for side in ("left", "bottom"):
        ax.spines[side].set_color(GRID)
    ax.tick_params(colors=TEXT_2)


def new_figure():
    fig, ax = plt.subplots(figsize=(7.5, 4.5), dpi=150)
    fig.patch.set_facecolor(SURFACE)
    return fig, ax


def line_by_d(test, column, title, ylabel, filename, label_offsets):
    """One line per method over D, with a direct label at the right end."""
    fig, ax = new_figure()
    for method in ORDER:
        label, colour, marker = STYLE[method]
        rows = sorted((r for r in test if r["method"] == method), key=lambda r: int(r["D"]))
        xs = [int(r["D"]) for r in rows]
        ys = [float(r[column]) for r in rows]
        dashes = (4, 3) if method == "exact" else (None, None)
        ax.plot(xs, ys, color=colour, linewidth=2, marker=marker, markersize=8,
                markeredgecolor=SURFACE, markeredgewidth=1.5, label=label,
                dashes=dashes if method == "exact" else (1, 0))
        ax.annotate(label, (xs[-1], ys[-1]), xytext=(10, label_offsets.get(method, 0) * 1.4),
                    textcoords="offset points", va="center", color=TEXT, fontsize=10)
    ax.set_xscale("log")
    ax.set_xticks(DS)
    ax.set_xticklabels([str(d) for d in DS])
    ax.set_xlim(800, 45000)
    style_axes(ax, title, "D (MIRFlickr distractors, log scale)", ylabel)
    ax.legend(frameon=False, loc="best", labelcolor=TEXT)
    fig.tight_layout()
    fig.savefig(os.path.join(PLOTS, filename), facecolor=SURFACE)
    plt.close(fig)


def tradeoff(validation, chosen, D, filename):
    """All validation settings at one D: query time against mAP@10."""
    fig, ax = new_figure()
    exact = [r for r in validation if r["method"] == "exact" and int(r["D"]) == D][0]
    exact_map = float(exact["map10"])

    for method in ["ivfflat", "lsh", "hypercube"]:
        label, colour, marker = STYLE[method]
        rows = [r for r in validation if r["method"] == method and int(r["D"]) == D]
        ax.scatter([float(r["time_ms"]) for r in rows], [float(r["map10"]) for r in rows],
                   s=40, color=colour, marker=marker, alpha=0.75, edgecolors=SURFACE,
                   linewidths=1, label=label + " (" + str(len(rows)) + " settings)")

    # chosen settings: ring around the point (the method is the colour and
    # shape inside the ring; the values are in the report's table)
    first = True
    for r in chosen:
        if int(r["D"]) == D and r["method"] != "exact":
            x, y = float(r["val_time_ms"]), float(r["val_map10"])
            ax.scatter([x], [y], s=200, facecolors="none", edgecolors=TEXT, linewidths=1.5, zorder=4,
                       label="chosen setting" if first else None)
            first = False

    label, colour, marker = STYLE["exact"]
    ax.scatter([float(exact["time_ms"])], [exact_map], s=90, color=colour, marker=marker,
               edgecolors=SURFACE, linewidths=1.5, label="Exact", zorder=3)
    ax.annotate("Exact", (float(exact["time_ms"]), exact_map), xytext=(8, 6),
                textcoords="offset points", color=TEXT, fontsize=10)
    ax.axhline(0.95 * exact_map, color=TEXT_2, linewidth=1, dashes=(4, 3))
    ax.annotate("95% of exact mAP@10 (selection target)", (1, 0.95 * exact_map),
                xycoords=("axes fraction", "data"), xytext=(-4, -4), textcoords="offset points",
                ha="right", va="top", color=TEXT_2, fontsize=9)

    style_axes(ax, "Quality against query time, validation, D = " + str(D),
               "average query time (ms)", "mAP@10")
    ax.legend(frameon=False, loc="lower right", labelcolor=TEXT, fontsize=9)
    fig.tight_layout()
    fig.savefig(os.path.join(PLOTS, filename), facecolor=SURFACE)
    plt.close(fig)


def main():
    os.makedirs(PLOTS, exist_ok=True)
    validation = read_csv("validation.csv")
    chosen = read_csv("chosen.csv")
    test = read_csv("test.csv")

    line_by_d(test, "map10", "mAP@10 on the test set (chosen parameters)", "mAP@10",
              "test_map_by_d.png", {"exact": 6, "ivfflat": -6, "lsh": 6, "hypercube": -6})
    line_by_d(test, "time_ms", "Average query time on the test set (chosen parameters)",
              "query time (ms)", "test_time_by_d.png", {"lsh": 6, "ivfflat": -2, "hypercube": -8})
    tradeoff(validation, chosen, 25000, "validation_tradeoff_D25000.png")
    tradeoff(validation, chosen, 1000, "validation_tradeoff_D1000.png")
    print("figures written to", PLOTS)


main()
