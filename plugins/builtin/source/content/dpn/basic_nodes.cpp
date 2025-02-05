#include <hex/api/content_registry.hpp>
#include <hex/api/localization.hpp>
#include <hex/helpers/utils.hpp>
#include <hex/ui/imgui_imhex_extensions.h>
#include <hex/data_processor/node.hpp>

#include <wolv/utils/core.hpp>

#include <nlohmann/json.hpp>

#include <imgui.h>

namespace hex::plugin::builtin {

    class NodeNullptr : public dp::Node {
    public:
        NodeNullptr() : Node("hex.builtin.nodes.constants.nullptr.header", { dp::Attribute(dp::Attribute::IOType::Out, dp::Attribute::Type::Buffer, "") }) { }

        void process() override {
            this->setBufferOnOutput(0, {});
        }
    };

    class NodeBuffer : public dp::Node {
    public:
        NodeBuffer() : Node("hex.builtin.nodes.constants.buffer.header", { dp::Attribute(dp::Attribute::IOType::Out, dp::Attribute::Type::Buffer, "") }) { }

        void drawNode() override {
            constexpr static int StepSize = 1, FastStepSize = 10;

            ImGui::PushItemWidth(100_scaled);
            ImGui::InputScalar("hex.builtin.nodes.constants.buffer.size"_lang, ImGuiDataType_U32, &this->m_size, &StepSize, &FastStepSize);
            ImGui::PopItemWidth();
        }

        void process() override {
            if (this->m_buffer.size() != this->m_size)
                this->m_buffer.resize(this->m_size, 0x00);

            this->setBufferOnOutput(0, this->m_buffer);
        }

        void store(nlohmann::json &j) const override {
            j = nlohmann::json::object();

            j["size"] = this->m_size;
            j["data"] = this->m_buffer;
        }

        void load(const nlohmann::json &j) override {
            this->m_size   = j.at("size");
            this->m_buffer = j.at("data").get<std::vector<u8>>();
        }

    private:
        u32 m_size = 1;
        std::vector<u8> m_buffer;
    };

    class NodeString : public dp::Node {
    public:
        NodeString() : Node("hex.builtin.nodes.constants.string.header", { dp::Attribute(dp::Attribute::IOType::Out, dp::Attribute::Type::Buffer, "") }) {

        }

        void drawNode() override {
            ImGui::InputTextMultiline("##string", this->m_value, ImVec2(150_scaled, 0), ImGuiInputTextFlags_AllowTabInput);
        }

        void process() override {
            this->setBufferOnOutput(0, hex::decodeByteString(this->m_value));
        }

        void store(nlohmann::json &j) const override {
            j = nlohmann::json::object();

            j["data"] = this->m_value;
        }

        void load(const nlohmann::json &j) override {
            this->m_value = j.at("data").get<std::string>();
        }

    private:
        std::string m_value;
    };

    class NodeInteger : public dp::Node {
    public:
        NodeInteger() : Node("hex.builtin.nodes.constants.int.header", { dp::Attribute(dp::Attribute::IOType::Out, dp::Attribute::Type::Integer, "") }) { }

        void drawNode() override {
            ImGui::PushItemWidth(100_scaled);
            ImGui::InputHexadecimal("##integer_value", &this->m_value);
            ImGui::PopItemWidth();
        }

        void process() override {
            this->setIntegerOnOutput(0, this->m_value);
        }

        void store(nlohmann::json &j) const override {
            j = nlohmann::json::object();

            j["data"] = this->m_value;
        }

        void load(const nlohmann::json &j) override {
            this->m_value = j.at("data");
        }

    private:
        u64 m_value = 0;
    };

    class NodeFloat : public dp::Node {
    public:
        NodeFloat() : Node("hex.builtin.nodes.constants.float.header", { dp::Attribute(dp::Attribute::IOType::Out, dp::Attribute::Type::Float, "") }) { }

        void drawNode() override {
            ImGui::PushItemWidth(100_scaled);
            ImGui::InputScalar("##floatValue", ImGuiDataType_Float, &this->m_value, nullptr, nullptr, "%f", ImGuiInputTextFlags_CharsDecimal);
            ImGui::PopItemWidth();
        }

        void process() override {
            this->setFloatOnOutput(0, this->m_value);
        }

        void store(nlohmann::json &j) const override {
            j = nlohmann::json::object();

            j["data"] = this->m_value;
        }

        void load(const nlohmann::json &j) override {
            this->m_value = j.at("data");
        }

    private:
        float m_value = 0;
    };

    class NodeRGBA8 : public dp::Node {
    public:
        NodeRGBA8() : Node("hex.builtin.nodes.constants.rgba8.header",
                          { dp::Attribute(dp::Attribute::IOType::Out, dp::Attribute::Type::Integer, "hex.builtin.nodes.constants.rgba8.output.r"),
                              dp::Attribute(dp::Attribute::IOType::Out, dp::Attribute::Type::Integer, "hex.builtin.nodes.constants.rgba8.output.g"),
                              dp::Attribute(dp::Attribute::IOType::Out, dp::Attribute::Type::Integer, "hex.builtin.nodes.constants.rgba8.output.b"),
                              dp::Attribute(dp::Attribute::IOType::Out, dp::Attribute::Type::Integer, "hex.builtin.nodes.constants.rgba8.output.a") }) { }

        void drawNode() override {
            ImGui::PushItemWidth(200_scaled);
            ImGui::ColorPicker4("##colorPicker", &this->m_color.Value.x, ImGuiColorEditFlags_AlphaBar);
            ImGui::PopItemWidth();
        }

        void process() override {
            this->setBufferOnOutput(0, wolv::util::toBytes<u8>(u8(this->m_color.Value.x * 0xFF)));
            this->setBufferOnOutput(1, wolv::util::toBytes<u8>(u8(this->m_color.Value.y * 0xFF)));
            this->setBufferOnOutput(2, wolv::util::toBytes<u8>(u8(this->m_color.Value.z * 0xFF)));
            this->setBufferOnOutput(3, wolv::util::toBytes<u8>(u8(this->m_color.Value.w * 0xFF)));
        }

        void store(nlohmann::json &j) const override {
            j = nlohmann::json::object();

            j["data"]      = nlohmann::json::object();
            j["data"]["r"] = this->m_color.Value.x;
            j["data"]["g"] = this->m_color.Value.y;
            j["data"]["b"] = this->m_color.Value.z;
            j["data"]["a"] = this->m_color.Value.w;
        }

        void load(const nlohmann::json &j) override {
            const auto &color = j.at("data");
            this->m_color = ImVec4(color.at("r"), color.at("g"), color.at("b"), color.at("a"));
        }

    private:
        ImColor m_color;
    };

    class NodeComment : public dp::Node {
    public:
        NodeComment() : Node("hex.builtin.nodes.constants.comment.header", {}) {

        }

        void drawNode() override {
            ImGui::InputTextMultiline("##string", this->m_comment, scaled(ImVec2(150, 100)));
        }

        void process() override {
        }

        void store(nlohmann::json &j) const override {
            j = nlohmann::json::object();

            j["comment"] = this->m_comment;
        }

        void load(const nlohmann::json &j) override {
            this->m_comment = j["comment"].get<std::string>();
        }

    private:
        std::string m_comment;
    };

    void registerBasicDataProcessorNodes() {
        ContentRegistry::DataProcessorNode::add<NodeInteger>("hex.builtin.nodes.constants", "hex.builtin.nodes.constants.int");
        ContentRegistry::DataProcessorNode::add<NodeFloat>("hex.builtin.nodes.constants", "hex.builtin.nodes.constants.float");
        ContentRegistry::DataProcessorNode::add<NodeNullptr>("hex.builtin.nodes.constants", "hex.builtin.nodes.constants.nullptr");
        ContentRegistry::DataProcessorNode::add<NodeBuffer>("hex.builtin.nodes.constants", "hex.builtin.nodes.constants.buffer");
        ContentRegistry::DataProcessorNode::add<NodeString>("hex.builtin.nodes.constants", "hex.builtin.nodes.constants.string");
        ContentRegistry::DataProcessorNode::add<NodeRGBA8>("hex.builtin.nodes.constants", "hex.builtin.nodes.constants.rgba8");
        ContentRegistry::DataProcessorNode::add<NodeComment>("hex.builtin.nodes.constants", "hex.builtin.nodes.constants.comment");
    }

}
