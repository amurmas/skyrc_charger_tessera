#include "skyrc_charger.h"
#include <cstdio>
#include <cstdlib>
#include <string>

namespace esphome::skyrc_charger {
using tessera::Font;
namespace ui = tessera::ui;

class ChannelTile : public tessera::Tile {
 public:
  explicit ChannelTile(const tessera::Plugin *p) : plugin_(p) {}
  void create(const tessera::TileContext &c) override {
    width_ = c.width; height_ = c.height;
    slot_ = c.options["slot"] | "A";
    const char *keys[] = {"voltage", "current", "capacity", "temperature", "status", "program", "battery_type", "charge_current", "start", "stop"};
    for (int i = 0; i < 10; ++i) entity_[i] = c.options[keys[i]] | "";
    title_ = ui::label(c.parent, Font::TITLE, theme::INK);
    readings_ = ui::label(c.parent, Font::BODY, theme::INK);
    config_ = ui::label(c.parent, Font::BODY, theme::MUTED);
    buttons_ = ui::label(c.parent, Font::BODY, theme::ACCENT);
    for (auto *label : {title_, readings_, config_, buttons_}) lv_obj_set_width(label, width_);
    lv_obj_set_style_text_align(title_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_align(buttons_, LV_TEXT_ALIGN_CENTER, 0);
  }
  void on_state(JsonObjectConst data) override {
    JsonObjectConst entities = data["entities"];
    const char *keys[] = {"voltage", "current", "capacity", "temperature", "status", "program", "battery_type", "charge_current", "start", "stop"};
    for (int i = 0; i < 10; ++i) {
      JsonObjectConst e = entities[keys[i]];
      state_[i] = e["state"] | "";
      unit_[i] = e["attributes"]["unit_of_measurement"] | "";
    }
    program_options_ = options_text(entities["program"]["attributes"]["options"]);
    battery_options_ = options_text(entities["battery_type"]["attributes"]["options"]);
    number_step_ = entities["charge_current"]["attributes"]["step"] | 0.0f;
    number_min_ = entities["charge_current"]["attributes"]["min"] | 0.0f;
    number_max_ = entities["charge_current"]["attributes"]["max"] | 10000.0f;
    paint();
  }
  void on_tick(uint32_t) override { paint(); }
  void on_theme() override {
    ui::set_color(title_, theme::INK); ui::set_color(readings_, theme::INK);
    ui::set_color(config_, theme::MUTED); ui::set_color(buttons_, theme::ACCENT);
  }
  void on_tap() override {
    const auto p = tessera::tap_point();
    if (p.y < 0 || height_ <= 0) return;
    const int row = p.y * 5 / height_;
    if (row == 4) {
      const int half = width_ / 2;
      const std::string &id = p.x < half ? entity_[8] : entity_[9];
      if (!id.empty()) tessera::action("button.press", id.c_str());
    } else if (row == 3) {
      if (p.x < width_ / 2) select_next(5, program_options_);
      else select_next(6, battery_options_);
    } else if (row == 2 && number_step_ > 0 && !entity_[7].empty()) {
      char *end = nullptr; float v = strtof(state_[7].c_str(), &end);
      if (end == state_[7].c_str()) return;
      float next = v + number_step_; if (next > number_max_) next = number_min_;
      JsonDocument fields; fields["value"] = next;
      tessera::action("number.set_value", entity_[7].c_str(), fields);
    }
  }
 private:
  static std::string options_text(JsonVariantConst options) {
    std::string out;
    for (JsonVariantConst option : options.as<JsonArrayConst>()) {
      const char *s = option.as<const char *>(); if (s && *s) { if (!out.empty()) out += '\n'; out += s; }
    }
    return out;
  }
  void select_next(int index, const std::string &options) {
    if (entity_[index].empty() || options.empty()) return;
    size_t at = 0;
    while (at < options.size()) {
      size_t end = options.find('\n', at); if (end == std::string::npos) end = options.size();
      if (options.substr(at, end - at) == state_[index]) { at = end < options.size() ? end + 1 : 0; break; }
      at = end < options.size() ? end + 1 : options.size();
    }
    size_t end = options.find('\n', at); std::string selected = options.substr(at, end == std::string::npos ? std::string::npos : end - at);
    if (!selected.empty()) { JsonDocument fields; fields["option"] = selected; tessera::action("select.select_option", entity_[index].c_str(), fields); }
  }
  void paint() {
    if (!title_) return;
    char b[240];
    std::snprintf(b, sizeof(b), "%s · %s", plugin_->text("slot"), slot_.c_str()); ui::set_text(title_, b);
    auto val = [this](int i) { return state_[i].empty() || state_[i] == "unavailable" ? std::string("--") : state_[i]; };
    std::snprintf(b, sizeof(b), "%s %s %s  ·  %s %s %s\n%s %s %s  ·  %s %s",
      plugin_->text("voltage_short"), val(0).c_str(), unit_[0].c_str(), plugin_->text("current_short"), val(1).c_str(), unit_[1].c_str(),
      plugin_->text("capacity_short"), val(2).c_str(), unit_[2].c_str(), plugin_->text("status_short"), val(4).c_str()); ui::set_text(readings_, b);
    std::snprintf(b, sizeof(b), "%s: %s  ·  %s: %s\n%s: %s %s  (+%.3g)", plugin_->text("program_short"), val(5).c_str(),
      plugin_->text("battery_short"), val(6).c_str(), plugin_->text("set_current"), val(7).c_str(), unit_[7].c_str(), number_step_);
    ui::set_text(config_, b);
    std::snprintf(b, sizeof(b), "◀ %s                       %s ▶", plugin_->text("start"), plugin_->text("stop")); ui::set_text(buttons_, b);
    const int th = ui::line_height(Font::TITLE), rh = ui::line_height(Font::BODY) * 2, ch = ui::line_height(Font::BODY) * 2, bh = ui::line_height(Font::BODY);
    const int total = th + rh + ch + bh + 16, y = (height_ - total) / 2 > 0 ? (height_ - total) / 2 : 0;
    lv_obj_set_pos(title_, 0, y); lv_obj_set_height(title_, th);
    lv_obj_set_pos(readings_, 0, y + th + 4); lv_obj_set_height(readings_, rh);
    lv_obj_set_pos(config_, 0, y + th + rh + 8); lv_obj_set_height(config_, ch);
    lv_obj_set_pos(buttons_, 0, y + th + rh + ch + 12); lv_obj_set_height(buttons_, bh);
  }
  const tessera::Plugin *plugin_; int width_ = 0, height_ = 0; float number_step_ = 0, number_min_ = 0, number_max_ = 10000;
  std::string slot_ = "A", entity_[10], state_[10], unit_[10], program_options_, battery_options_;
  lv_obj_t *title_{}, *readings_{}, *config_{}, *buttons_{};
};
void SkyrcCharger::setup() { add_tile("channel", [this]() { return new ChannelTile(this); }); }
}  // namespace esphome::skyrc_charger
