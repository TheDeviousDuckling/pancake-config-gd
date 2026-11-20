// Auto Deco - decorates the exposed edges of selected solid blocks.
// NOTE: object IDs in the theme tables below are STARTING POINTS.
// Verify/replace them with IDs you like (select an object in the editor
// and check its ID in the object info / "Edit Object" panel).
#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <set>
#include <vector>
#include <random>

using namespace geode::prelude;

struct Piece { int id; float dx, dy, rot, scale; };
struct Theme {
	const char* name;
	std::vector<Piece> top, bottom, left, right;
};

static std::vector<Theme> const& themes() {
	static std::vector<Theme> t = {
		{ "Acheron",
			{ {1730, 0, 22, 0, 0.8f}, {1731, 0, 20, 0, 1.0f} },       // top (placeholder IDs)
			{ {1732, 0, -20, 180, 0.8f} },                            // bottom
			{ {1733, -20, 0, 270, 0.7f} },                            // left
			{ {1733, 20, 0, 90, 0.7f} } },                            // right
		{ "Space",
			{ {1734, 0, 24, 0, 0.9f} },
			{ {1734, 0, -24, 180, 0.9f} },
			{ {1735, -22, 0, 0, 0.8f} },
			{ {1735, 22, 0, 0, 0.8f} } },
		{ "Water",
			{ {1736, 0, 18, 0, 1.0f}, {1737, 0, 20, 0, 0.7f} },
			{ {1738, 0, -18, 180, 0.9f} },
			{ {1739, -18, 0, 0, 0.8f} },
			{ {1739, 18, 0, 0, 0.8f} } },
		{ "Grasslands",
			{ {1740, 0, 18, 0, 1.0f}, {1741, 0, 20, 0, 0.8f} },
			{ {1742, 0, -18, 180, 0.8f} },
			{ },
			{ } },
	};
	return t;
}

static int g_theme = 0;

static std::pair<int,int> cell(CCPoint p) {
	return { (int)std::floor(p.x / 30.f), (int)std::floor(p.y / 30.f) };
}

class $modify(AutoDecoUI, EditorUI) {
	struct Fields { CCLabelBMFont* themeLabel = nullptr; };

	bool init(LevelEditorLayer* lel) {
		if (!EditorUI::init(lel)) return false;

		auto menu = CCMenu::create();
		menu->setID("auto-deco-menu"_spr);
		auto win = CCDirector::get()->getWinSize();
		menu->setPosition({ win.width - 70.f, win.height - 60.f });
		menu->setLayout(ColumnLayout::create()->setAxisReverse(true)->setGap(4.f));

		auto themeBtn = CCMenuItemSpriteExtra::create(
			ButtonSprite::create(themes()[g_theme].name, 90, true, "bigFont.fnt", "GJ_button_04.png", 30.f, .5f),
			this, menu_selector(AutoDecoUI::onCycleTheme));
		themeBtn->setID("theme-btn");
		auto decoBtn = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("Auto Deco", 90, true, "bigFont.fnt", "GJ_button_01.png", 30.f, .5f),
			this, menu_selector(AutoDecoUI::onAutoDeco));
		menu->addChild(themeBtn);
		menu->addChild(decoBtn);
		menu->updateLayout();
		this->addChild(menu, 100);
		return true;
	}

	void onCycleTheme(CCObject* sender) {
		g_theme = (g_theme + 1) % themes().size();
		auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
		if (auto spr = typeinfo_cast<ButtonSprite*>(btn->getNormalImage()))
			spr->setString(themes()[g_theme].name);
	}

	void onAutoDeco(CCObject*) {
		auto selected = this->getSelectedObjectsOfCCArray();
		if (!selected || selected->count() == 0) {
			FLAlertLayer::create("Auto Deco", "Select some solid blocks first.", "OK")->show();
			return;
		}

		std::set<std::pair<int,int>> occupied;
		std::vector<GameObject*> blocks;
		for (auto obj : CCArrayExt<GameObject*>(selected)) {
			occupied.insert(cell(obj->getPosition()));
			if (obj->m_objectType == GameObjectType::Solid) blocks.push_back(obj);
		}

		auto const& theme = themes()[g_theme];
		int density = Mod::get()->getSettingValue<int64_t>("density");
		std::mt19937 rng(std::random_device{}());
		std::uniform_int_distribution<int> pct(1, 100);
		int created = 0;

		auto place = [&](GameObject* base, std::vector<Piece> const& pieces) {
			if (pieces.empty() || pct(rng) > density) return;
			auto& p = pieces[rng() % pieces.size()];
			auto pos = base->getPosition() + CCPoint{ p.dx, p.dy };
			auto o = m_editorLayer->createObject(p.id, pos, false);
			if (!o) return;
			o->setRotation(p.rot);
			o->setScale(p.scale);
			created++;
		};

		for (auto b : blocks) {
			auto c = cell(b->getPosition());
			if (!occupied.count({ c.first, c.second + 1 })) place(b, theme.top);
			if (!occupied.count({ c.first, c.second - 1 })) place(b, theme.bottom);
			if (!occupied.count({ c.first - 1, c.second })) place(b, theme.left);
			if (!occupied.count({ c.first + 1, c.second })) place(b, theme.right);
		}

		Notification::create(fmt::format("Auto Deco: added {} objects", created),
			NotificationIcon::Success)->show();
	}
};
