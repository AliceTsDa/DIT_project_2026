#pragma once

// Simple descriptor statistics over a group of images
struct GroupStats {
    int images;
    long descriptors;
    int minDescriptors;
    int maxDescriptors;
};
