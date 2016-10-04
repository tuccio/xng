#include "scene_graph_page.hpp"

#include <xng/math.hpp>
#include <xng/geometry.hpp>

#include "wx_helpers.hpp"

#include <deque>

using namespace xng::math;
using namespace xng::graphics;
using namespace xng::editor;
using namespace xng::editor::detail;

wxDEFINE_EVENT(XNG_EDITOR_EVENT_REFRESH, wxCommandEvent);

scene_graph_page::scene_graph_page(scene_graph * g, wxWindow * parent) :
	wxWindow(parent, wxID_ANY),
	m_sceneGraph(nullptr),
	m_selectedNode(nullptr)
{
	const int padding = 5;

	wxBoxSizer * sizer = xng_new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer * localTranslationBox = xng_new wxStaticBoxSizer(wxVERTICAL, this, _("Local translation"));
	wxBoxSizer       * localTranslationFields = xng_new wxBoxSizer(wxHORIZONTAL);

	wxStaticBoxSizer * localScaleBox = xng_new wxStaticBoxSizer(wxVERTICAL, this, _("Local scale"));
	wxBoxSizer       * localScaleFields = xng_new wxBoxSizer(wxHORIZONTAL);

	wxStaticBoxSizer * localRotationBox = xng_new wxStaticBoxSizer(wxVERTICAL, this, _("Local rotation"));
	wxBoxSizer       * localRotationFields = xng_new wxBoxSizer(wxHORIZONTAL);

	m_sgTree     = xng_new wxTreeCtrl(this);
	m_sceneGraph = g;

	m_sceneGraph->add_observer(this);

	update_scene_graph();
	set_selected_node(m_sceneGraph->get_root());

	/* Local translation */

	auto localTranslationX = xng_new wx_spin<float>(
		[&]()
	{
		if (m_selectedNode)
		{
			return m_selectedNode->get_local_translation().x;
		}
		return 0.f;
	},
		[&](float x)
	{
		if (m_selectedNode)
		{
			auto v = m_selectedNode->get_local_translation();
			v.x = x;
			m_selectedNode->set_local_translation(v);
		}
	}, this, wxID_ANY, -FLT_MAX, FLT_MAX, .1f);

	auto localTranslationY = xng_new wx_spin<float>(
		[&]()
	{
		if (m_selectedNode)
		{
			return m_selectedNode->get_local_translation().y;
		}
		return 0.f;
	},
		[&](float x)
	{
		if (m_selectedNode)
		{
			auto v = m_selectedNode->get_local_translation();
			v.y = x;
			m_selectedNode->set_local_translation(v);
		}
	}, this, wxID_ANY, -FLT_MAX, FLT_MAX, .1f);

	auto localTranslationZ = xng_new wx_spin<float>(
		[&]()
	{
		if (m_selectedNode)
		{
			return m_selectedNode->get_local_translation().z;
		}
		return 0.f;
	},
		[&](float x)
	{
		if (m_selectedNode)
		{
			auto v = m_selectedNode->get_local_translation();
			v.z = x;
			m_selectedNode->set_local_translation(v);
		}
	}, this, wxID_ANY, -FLT_MAX, FLT_MAX, .1f);

	/* Local scale */

	auto localScaleX = xng_new wx_spin<float>(
		[&]()
	{
		if (m_selectedNode)
		{
			return m_selectedNode->get_local_scale().x;
		}
		return 0.f;
	},
		[&](float x)
	{
		if (m_selectedNode)
		{
			auto v = m_selectedNode->get_local_scale();
			v.x = x;
			m_selectedNode->set_local_scale(v);
		}
	}, this, wxID_ANY, -FLT_MAX, FLT_MAX, .1f);

	auto localScaleY = xng_new wx_spin<float>(
		[&]()
	{
		if (m_selectedNode)
		{
			return m_selectedNode->get_local_scale().y;
		}
		return 0.f;
	},
		[&](float x)
	{
		if (m_selectedNode)
		{
			auto v = m_selectedNode->get_local_scale();
			v.y = x;
			return m_selectedNode->set_local_scale(v);
		}
	}, this, wxID_ANY, -FLT_MAX, FLT_MAX, .1f);

	auto localScaleZ = xng_new wx_spin<float>(
		[&]()
	{
		if (m_selectedNode)
		{
			return m_selectedNode->get_local_scale().z;
		}
		return 0.f;
	},
		[&](float x)
	{
		if (m_selectedNode)
		{
			auto v = m_selectedNode->get_local_scale();
			v.z = x;
			return m_selectedNode->set_local_scale(v);
		}
	}, this, wxID_ANY, -FLT_MAX, FLT_MAX, .1f);

	/* Local rotation */

	auto localRotationX = xng_new wx_spin<float>(
		[&]()
	{
		if (m_selectedNode)
		{
			quaternion q = m_selectedNode->get_local_rotation();
			float3     e = to_euler(q);
			return to_degrees(e.x);
		}
		return 0.f;
	},
		[&](float x)
	{
		if (m_selectedNode)
		{
			if (m_selectedNode)
			{
				quaternion q = m_selectedNode->get_local_rotation();
				float3     e = to_euler(q);
				e.x = to_radians(x);
				m_selectedNode->set_local_rotation(to_quaternion(e));
			}
		}
	}, this, wxID_ANY, -FLT_MAX, FLT_MAX, .1f);

	auto localRotationY = xng_new wx_spin<float>(
		[&]()
	{
		if (m_selectedNode)
		{
			quaternion q = m_selectedNode->get_local_rotation();
			float3     e = to_euler(q);
			return to_degrees(e.y);
		}
		return 0.f;
	},
		[&](float x)
	{
		if (m_selectedNode)
		{
			quaternion q = m_selectedNode->get_local_rotation();
			float3     e = to_euler(q);
			e.y = to_radians(x);
			m_selectedNode->set_local_rotation(to_quaternion(e));
		}
	}, this, wxID_ANY, -FLT_MAX, FLT_MAX, .1f);

	auto localRotationZ = xng_new wx_spin<float>(
		[&]()
	{
		if (m_selectedNode)
		{
			quaternion q = m_selectedNode->get_local_rotation();
			float3     e = to_euler(q);
			return to_degrees(e.z);
		}
		return 0.f;
	},
		[&](float x)
	{
		if (m_selectedNode)
		{
			quaternion q = m_selectedNode->get_local_rotation();
			float3     e = to_euler(q);
			e.z = to_radians(x);
			m_selectedNode->set_local_rotation(to_quaternion(e));
		}
	}, this, wxID_ANY, -FLT_MAX, FLT_MAX, .1f);

	m_sgTree->Bind(wxEVT_TREE_BEGIN_DRAG,
		[&](wxTreeEvent & event)
	{
		wxTreeItemId item = event.GetItem();

		if (item != m_sgTree->GetRootItem())
		{
			m_draggedItem = item;
			event.Allow();
		}
	});

	m_sgTree->Bind(wxEVT_TREE_END_DRAG,
		[&](wxTreeEvent & event)
	{
		wxTreeItemId parent = event.GetItem();
		wxTreeItemId item = m_draggedItem;

		auto parentIt = m_itemNodeMap.find(parent);
		auto nodeIt = m_itemNodeMap.find(item);

		if (parentIt != m_itemNodeMap.end() &&
			nodeIt != m_itemNodeMap.end())
		{
			scene_graph_node * parent = parentIt->second;
			scene_graph_node * node = nodeIt->second;

			if (parent->is_ancestor(node))
			{
				XNG_LOG("Editor", "Can't complete action: no loops allowed in the scene graph, cannot attach an ancestor to a descendant.");
			}
			else if (parent != node)
			{
				remove_branch(node);
				node->set_parent(parent);
				add_branch(node);
			}
		}
	});

	/* Size settings */

	localTranslationX->SetMinSize(wxSize(64, 24));
	localTranslationY->SetMinSize(wxSize(64, 24));
	localTranslationZ->SetMinSize(wxSize(64, 24));

	localScaleX->SetMinSize(wxSize(64, 24));
	localScaleY->SetMinSize(wxSize(64, 24));
	localScaleZ->SetMinSize(wxSize(64, 24));

	localRotationX->SetMinSize(wxSize(64, 24));
	localRotationY->SetMinSize(wxSize(64, 24));
	localRotationZ->SetMinSize(wxSize(64, 24));

	/* Add elements */

	//localTranslation->Add(new wxStaticText(sg, wxID_ANY, _("Translation")), 0, wxEXPAND | wxALL, padding);
	localTranslationFields->Add(localTranslationX, 1, wxEXPAND | wxALL);
	localTranslationFields->Add(localTranslationY, 1, wxEXPAND | wxALL);
	localTranslationFields->Add(localTranslationZ, 1, wxEXPAND | wxALL);

	localScaleFields->Add(localScaleX, 1, wxEXPAND | wxALL);
	localScaleFields->Add(localScaleY, 1, wxEXPAND | wxALL);
	localScaleFields->Add(localScaleZ, 1, wxEXPAND | wxALL);

	localRotationFields->Add(localRotationX, 1, wxEXPAND | wxALL);
	localRotationFields->Add(localRotationY, 1, wxEXPAND | wxALL);
	localRotationFields->Add(localRotationZ, 1, wxEXPAND | wxALL);

	localTranslationBox->Add(localTranslationFields, 1, wxEXPAND | wxALL);
	localScaleBox->Add(localScaleFields, 1, wxEXPAND | wxALL);
	localRotationBox->Add(localRotationFields, 1, wxEXPAND | wxALL);

	sizer->Add(m_sgTree, 1, wxEXPAND | wxALL, padding);

	sizer->Add(localTranslationBox, 0, wxEXPAND | wxALL, padding);
	sizer->Add(localScaleBox, 0, wxEXPAND | wxALL, padding);
	sizer->Add(localRotationBox, 0, wxEXPAND | wxALL, padding);

	SetSizerAndFit(sizer);

	m_sgTree->Bind(wxEVT_TREE_SEL_CHANGED,
		[&] (wxTreeEvent & event)
	{
		wxTreeItemId item = event.GetItem();
		m_selectedNode = m_itemNodeMap[item];
		update_selected_object();
	});

	m_sgTree->Bind(XNG_EDITOR_EVENT_REFRESH,
		[=](wxCommandEvent & event)
	{
		localTranslationX->update_value();
		localTranslationY->update_value();
		localTranslationZ->update_value();
		localScaleX->update_value();
		localScaleY->update_value();
		localScaleZ->update_value();
		localRotationX->update_value();
		localRotationY->update_value();
		localRotationZ->update_value();
	});
}

scene_graph_page::~scene_graph_page(void)
{
	m_sceneGraph->remove_observer(this);
}

void scene_graph_page::update_scene_graph(void)
{
	m_sgTree->DeleteAllItems();
	add_branch(m_sceneGraph->get_root());
}

void scene_graph_page::update_selected_object(void)
{
	wxPostEvent(m_sgTree, wxCommandEvent(XNG_EDITOR_EVENT_REFRESH));
}

scene_graph_node * scene_graph_page::get_selected_node(void) const
{
	return m_selectedNode;
}

void scene_graph_page::set_selected_node(scene_graph_node * node)
{
	auto it = m_nodeItemMap.find(node);
}

void scene_graph_page::on_create(scene_graph * sg, scene_graph_node * node)
{

}

void scene_graph_page::on_destroy(scene_graph * sg, scene_graph_node * node)
{

}

void scene_graph_page::on_move(scene_graph * sg, scene_graph_node * node, const float4x4 & lastTransform, const float4x4 & newTransform)
{
	if (node == m_selectedNode)
	{
		update_selected_object();
	}
}

void scene_graph_page::add_branch(scene_graph_node * root)
{
	std::deque<scene_graph_node*> stack = { root };

	do
	{
		scene_graph_node * node   = stack.front();
		scene_graph_node * parent = node->get_parent();

		std::string name = node->get_name();
		wxTreeItemId id;

		if (parent)
		{
			id = m_sgTree->AppendItem(
				m_nodeItemMap[parent],
				name.empty() ? _("<unnamed node>") : name.c_str());
		}
		else
		{
			id = m_sgTree->AddRoot(_("<root>"));
		}

		m_nodeItemMap[node] = id;
		m_itemNodeMap[id] = node;

		std::copy(node->begin(), node->end(), std::back_inserter(stack));
		stack.pop_front();
	} while (!stack.empty());
}

void scene_graph_page::remove_branch(scene_graph_node * root)
{
	std::deque<scene_graph_node*> stack = { root };

	do
	{
		scene_graph_node * node = stack.front();

		auto nodeItemIt = m_nodeItemMap.find(node);

		wxTreeItemId id = nodeItemIt->second;

		m_sgTree->Delete(id);

		m_nodeItemMap.erase(nodeItemIt);
		m_itemNodeMap.erase(m_itemNodeMap.find(id));

		std::copy(node->begin(), node->end(), std::back_inserter(stack));
		stack.pop_front();
	} while (!stack.empty());
}