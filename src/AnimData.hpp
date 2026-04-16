#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <cstdio>
#include <iostream>

struct AnimData {
    std::vector<sf::Texture> frames;
    float frameTime = 0.1f;   // seconds shown per frame

    /**
     * @param folder   Directory holding the frame images
     * @param prefix   Filename prefix  (e.g. "tile", "idle_", "1_atk_")
     * @param start    First frame index (inclusive)
     * @param end      Last  frame index (inclusive)
     * @param padWidth Zero-padding width; 0 = no padding
     * @return true if at least one frame loaded successfully
     */
    bool load(const std::string& folder,
              const std::string& prefix,
              int start, int end,
              int padWidth = 3)
    {
        frames.clear();
        char buf[512];
        for (int i = start; i <= end; ++i) {
            if (padWidth > 0)
                std::snprintf(buf, sizeof buf, "%s/%s%0*d.png",
                              folder.c_str(), prefix.c_str(), padWidth, i);
            else
                std::snprintf(buf, sizeof buf, "%s/%s%d.png",
                              folder.c_str(), prefix.c_str(), i);
            sf::Texture t;
            if (!t.loadFromFile(buf))
                std::cerr << "[WARN] AnimData: cannot load " << buf << '\n';
            else
                frames.push_back(std::move(t));
        }
        return !frames.empty();
    }

    bool empty()  const { return frames.empty(); }
    int  count()  const { return static_cast<int>(frames.size()); }

    // Safe cyclic access
    const sf::Texture& get(int i) const {
        int n = count();
        return frames[((i % n) + n) % n];
    }
};
