# Experiments (K = 256, S = 500, seed = 1)

Selection on validation: fastest setting with mAP@10 >= 0.95 x Exact
(else best mAP@10). Test query times: average of 3 runs.

## Chosen parameters (validation)

| method | D | parameters | val mAP@10 | val time (ms) | exact mAP@10 | rule |
|---|---|---|---|---|---|---|
| exact | 1000 | - | 0.8000 | 0.2847 | 0.8000 | fastest >= 0.95 x exact |
| exact | 5000 | - | 0.7840 | 1.4144 | 0.7840 | fastest >= 0.95 x exact |
| exact | 10000 | - | 0.7732 | 3.1201 | 0.7732 | fastest >= 0.95 x exact |
| exact | 25000 | - | 0.7324 | 6.7594 | 0.7324 | fastest >= 0.95 x exact |
| hypercube | 1000 | -kproj 8 -w 0.4 -M 446 -probes 50 | 0.7747 | 0.1150 | 0.8000 | fastest >= 0.95 x exact |
| hypercube | 5000 | -kproj 8 -w 0.4 -M 2046 -probes 500 | 0.7631 | 0.5428 | 0.7840 | fastest >= 0.95 x exact |
| hypercube | 10000 | -kproj 12 -w 0.4 -M 4046 -probes 500 | 0.7395 | 1.1950 | 0.7732 | fastest >= 0.95 x exact |
| hypercube | 25000 | -kproj 12 -w 0.4 -M 5023 -probes 50 | 0.7026 | 1.5883 | 0.7324 | fastest >= 0.95 x exact |
| ivfflat | 1000 | -kclusters 68 -nprobe 5 | 0.7913 | 0.1692 | 0.8000 | fastest >= 0.95 x exact |
| ivfflat | 5000 | -kclusters 144 -nprobe 5 | 0.7675 | 0.3534 | 0.7840 | fastest >= 0.95 x exact |
| ivfflat | 10000 | -kclusters 202 -nprobe 10 | 0.7499 | 0.9997 | 0.7732 | fastest >= 0.95 x exact |
| ivfflat | 25000 | -kclusters 318 -nprobe 10 | 0.7150 | 1.7128 | 0.7324 | fastest >= 0.95 x exact |
| lsh | 1000 | -k 6 -L 5 -w 0.2 | 0.7658 | 0.1235 | 0.8000 | fastest >= 0.95 x exact |
| lsh | 5000 | -k 6 -L 5 -w 0.2 | 0.7587 | 0.6209 | 0.7840 | fastest >= 0.95 x exact |
| lsh | 10000 | -k 6 -L 5 -w 0.2 | 0.7461 | 1.3154 | 0.7732 | fastest >= 0.95 x exact |
| lsh | 25000 | -k 4 -L 10 -w 0.1 | 0.7038 | 2.8701 | 0.7324 | fastest >= 0.95 x exact |

## Test results

| method | D | Recall@5 | Recall@10 | mAP@10 | time (ms) | compared (%) | < 10 results | mAP illum. | mAP viewp. |
|---|---|---|---|---|---|---|---|---|---|
| exact | 1000 | 0.7304 | 0.7826 | 0.7212 | 0.3100 | 100.0 | 0 | 0.6948 | 0.7454 |
| hypercube | 1000 | 0.6783 | 0.7304 | 0.6743 | 0.1149 | 40.0 | 0 | 0.6979 | 0.6527 |
| ivfflat | 1000 | 0.7043 | 0.7478 | 0.6959 | 0.1303 | 38.3 | 0 | 0.6464 | 0.7412 |
| lsh | 1000 | 0.6696 | 0.7217 | 0.6714 | 0.1294 | 39.8 | 1 | 0.6494 | 0.6915 |
| exact | 5000 | 0.6870 | 0.7565 | 0.6960 | 1.3752 | 100.0 | 0 | 0.6842 | 0.7067 |
| hypercube | 5000 | 0.6609 | 0.7217 | 0.6640 | 0.5361 | 40.0 | 0 | 0.6849 | 0.6448 |
| ivfflat | 5000 | 0.6522 | 0.7217 | 0.6619 | 0.2971 | 17.6 | 0 | 0.6130 | 0.7067 |
| lsh | 5000 | 0.6609 | 0.7130 | 0.6661 | 0.6119 | 39.7 | 0 | 0.6464 | 0.6841 |
| exact | 10000 | 0.6870 | 0.7478 | 0.6915 | 2.6964 | 100.0 | 0 | 0.6788 | 0.7031 |
| hypercube | 10000 | 0.6696 | 0.7304 | 0.6758 | 1.1771 | 40.0 | 0 | 0.6688 | 0.6823 |
| ivfflat | 10000 | 0.6783 | 0.7391 | 0.6828 | 1.0716 | 27.4 | 0 | 0.6606 | 0.7031 |
| lsh | 10000 | 0.6609 | 0.7043 | 0.6642 | 1.3265 | 38.9 | 0 | 0.6445 | 0.6823 |
| exact | 25000 | 0.6696 | 0.7304 | 0.6837 | 6.6881 | 100.0 | 0 | 0.6777 | 0.6893 |
| hypercube | 25000 | 0.6261 | 0.6957 | 0.6448 | 1.5497 | 20.0 | 0 | 0.6106 | 0.6761 |
| ivfflat | 25000 | 0.6696 | 0.7304 | 0.6837 | 1.9566 | 18.8 | 0 | 0.6777 | 0.6893 |
| lsh | 25000 | 0.6522 | 0.7043 | 0.6614 | 2.7088 | 27.3 | 0 | 0.6309 | 0.6893 |
