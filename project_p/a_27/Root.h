#pragma once
#include<vector>
#include<random>
#include "Model.h"

//¸ðµ¨
std::vector<Model*> models;

std::random_device rd;
std::mt19937 gen(rd());
uniform_real_distribution<> snow_location_dis_x(-0.65, 0.65);
