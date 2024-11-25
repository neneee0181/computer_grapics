#pragma once
#include<vector>
#include<random>
#include "Model.h"

//¸ðµ¨
std::vector<Model*> models;

std::random_device rd;
std::mt19937 gen(rd());