/*
Minetest
Copyright (C) 2010-2013 celeron55, Perttu Ahola <celeron55@gmail.com>
Copyright (C) 2017 numzero, Lobachevskiy Vitaliy <numzer0@yandex.ru>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <iostream>
#include "core.h"
#include "plain.h"
#include "client/shadows/dynamicshadowsrender.h"
#include "settings.h"
#include "client/camera.h"
#include "client/client.h"
#include "client/content_cao.h"
#include "client/clientmap.h"
#include "client/hud.h"
#include "nodedef.h"
#include "client/mapblock_mesh.h"
#include "script/scripting_client.h"

RenderingCore::RenderingCore(IrrlichtDevice *_device, Client *_client, Hud *_hud,
		ShadowRenderer *_shadow_renderer, RenderPipeline *_pipeline, v2f _virtual_size_scale)
	: device(_device), client(_client), hud(_hud), shadow_renderer(_shadow_renderer),
	pipeline(_pipeline), virtual_size_scale(_virtual_size_scale)
{
}

RenderingCore::~RenderingCore()
{
	delete pipeline;
	delete shadow_renderer;
}

void RenderingCore::draw(video::SColor _skycolor, bool _show_hud, bool _show_minimap,
		bool _draw_wield_tool, bool _draw_crosshair)
{
	v2u32 screensize = device->getVideoDriver()->getScreenSize();
	virtual_size = v2u32(screensize.X * virtual_size_scale.X, screensize.Y * virtual_size_scale.Y);

	PipelineContext context(device, client, hud, shadow_renderer, _skycolor, screensize);
	context.draw_crosshair = _draw_crosshair;
	context.draw_wield_tool = _draw_wield_tool;
	context.show_hud = _show_hud;

	pipeline->reset(context);
	//pipeline->run(context);
	Draw3D(context);
	drawHitbox();
	DrawHUD(context);
	DrawWield(context);
}


v2u32 RenderingCore::getVirtualSize() const
{
	return virtual_size;
}

void RenderingCore::drawHitbox()
{
	auto driver = device->getVideoDriver();

	draw_player_hitbox = g_settings->getBool("debug_hitbox");

	ClientEnvironment &env = client->getEnv();
	Camera *camera = client->getCamera();
	
	video::SMaterial material, oldmaterial;
 	oldmaterial = driver->getMaterial2D();
	material.setFlag(video::EMF_LIGHTING, false);
	material.setFlag(video::EMF_BILINEAR_FILTER, false);
	material.setFlag(video::EMF_ZBUFFER, false);
	material.setFlag(video::EMF_ZWRITE_ENABLE, false);
	
	driver->setMaterial(material);
	video::SColor color = video::SColor(255, 255, 255, 255);
	v3f camera_offset = intToFloat(camera->getOffset(), BS);
	if (draw_player_hitbox  && client->checkPrivilege("debug")) {
		auto allObjects = env.getAllActiveObjects();
		for (auto &it : allObjects) {
			ClientActiveObject *cao = it.second;
			if (cao->isLocalPlayer() || cao->getParent())
				continue;
			GenericCAO *obj = dynamic_cast<GenericCAO *>(cao);
			if (! obj)
				continue;
			bool hitbox = obj->isPlayer() ? draw_player_hitbox : false;
			if (!(hitbox))
				continue;
			aabb3f box;
			if (! obj->getSelectionBox(&box))
				continue;
			v3f pos = obj->getPosition() - camera_offset;
			box.MinEdge += pos;
			box.MaxEdge += pos;
			if (hitbox)
				driver->draw3DBox(box, color);
		}
	}
	driver->setMaterial(oldmaterial);
}

void RenderingCore::Draw3D(PipelineContext &context)
{
	context.device->getSceneManager()->drawAll();
	context.device->getVideoDriver()->setTransform(video::ETS_WORLD, core::IdentityMatrix);
	if (!context.show_hud)
		return;
	context.hud->drawBlockBounds();
	context.hud->drawSelectionMesh();
}

void RenderingCore::DrawWield(PipelineContext &context)
{
	if (context.draw_wield_tool)
		context.client->getCamera()->drawWieldedTool();
}

void RenderingCore::DrawHUD(PipelineContext &context)
{
	if (context.show_hud) {
		if (context.shadow_renderer)
			context.shadow_renderer->drawDebug();

		context.hud->resizeHotbar();

		if (context.draw_crosshair)
			context.hud->drawCrosshair();

		context.hud->drawHotbar(context.client->getEnv().getLocalPlayer()->getWieldIndex());
		context.hud->drawLuaElements(context.client->getCamera()->getOffset());
		context.client->getCamera()->drawNametags();
	}
	context.device->getGUIEnvironment()->drawAll();
}

