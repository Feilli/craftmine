#pragma once

#include <random>
#include <vector>

class Perlin {
public:
    Perlin(unsigned int seed = std::random_device{}());
    ~Perlin();

    double Noise(double x, double y, double z) const;

private:
    std::vector<int> m_P;

    static double Fade(double t);
    static double Lerp(double t, double a, double b);
    static double Grad(int hash, double x, double y, double z);
};