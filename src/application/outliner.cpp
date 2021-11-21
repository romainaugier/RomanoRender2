#include "application/outliner.h"

void Outliner::Draw() noexcept
{
	ImGui::Begin("Outliner");
	{
		if (ImGui::BeginTabBar(""))
		{
			// Objects

			if (ImGui::BeginTabItem("Objects"))
			{
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					ImGui::OpenPopup("add_object_popup");
				}

				if (ImGui::BeginPopup("add_object_popup"))
				{
					if (ImGui::MenuItem("Import Object"))
					{
						ifd::FileDialog::Instance().Open("ObjOpenDialog", "Select an .obj file", "Obj File (*.obj){.obj},.*");
					}

					ImGui::EndPopup();
				}

				if (ifd::FileDialog::Instance().IsDone("ObjOpenDialog"))
				{
					if (ifd::FileDialog::Instance().HasResult())
					{
						const auto& res = ifd::FileDialog::Instance().GetResults();
						const std::string path = res[0].u8string();

						const std::string baseFilename = path.substr(path.find_last_of("/\\") + 1);
						const std::string::size_type const p(baseFilename.find_last_of('.'));
						const std::string objectPath = baseFilename.substr(0, p);

						this->objects.emplace(objectPath, ObjectHolder(path));
						this->renderer->LoadObject(path);
						this->renderer->RebuildScene();
					}

					ifd::FileDialog::Instance().Close();
				}

				if (!this->objects.empty())
				{
					for (const auto& [objHolderKey, objHolder] : this->objects)
					{
						if (ImGui::TreeNode(objHolderKey.c_str()))
						{
							for (const auto& [objKey, obj] : this->renderer->sceneObjects)
							{
								if (obj.scenePath == objHolderKey)
								{
									ImGui::Selectable(obj.name.c_str());
								}
							}

							ImGui::TreePop();
						}
					}
				}

				ImGui::EndTabItem();
			}

			// Shaders

			if (ImGui::BeginTabItem("Shader"))
			{
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					ImGui::OpenPopup("add_shader_popup");
				}

				if (ImGui::BeginPopup("add_shader_popup"))
				{
					if (ImGui::MenuItem("OSL Shader"))
					{
						char buffer[256];
						sprintf_s(buffer, "NewOSLShader%d", this->numShaders);
						
						this->shaders.emplace(buffer, ShaderHolder(this->numShadersIdIncr));

						this->numShaders++; this->numShadersIdIncr++;
					}

					ImGui::EndPopup();
				}

				if (!this->shaders.empty())
				{
					for (const auto& [key, value] : this->shaders)
					{
						ImGui::Selectable(key.c_str());
					}
				}

				ImGui::EndTabItem();
			}

			// Settings

			if (ImGui::BeginTabItem("Settings"))
			{
				if (ImGui::CollapsingHeader("Shading"))
				{
					static int bounces;

					ImGui::Text("Bounces");
					ImGui::SameLine();
					ImGui::InputInt("", &bounces);
				}
				if (ImGui::CollapsingHeader("Sampling"))
				{
					static int samples;

					ImGui::Text("Samples");
					ImGui::SameLine();
					ImGui::InputInt("", &samples);
				}
				if (ImGui::CollapsingHeader("File"))
				{
					static ImVec2 renderSize;

					ImGui::Text("Render Size");
					ImGui::SameLine();
					ImGui::InputFloat2("", &renderSize[0]);
				}

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}

	ImGui::End();
}