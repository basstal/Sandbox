#include "pch.hpp"

#include "Stats.hpp"
Sandbox::Stats::Stats() { name = "Stats"; }

void Sandbox::Stats::OnGui() { ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate); }
