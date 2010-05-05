/*This file is generated by luagen.*/
#include "lua_ftk_combo_box.h"
#include "lua_ftk_callbacks.h"

static void tolua_reg_types (lua_State* L)
{
	tolua_usertype(L, "FtkComboBox");
}

static int lua_ftk_combo_box_create(lua_State* L)
{
	tolua_Error err = {0};
	FtkComboBox* retv;
	FtkWidget* parent;
	int x;
	int y;
	int width;
	int height;
	int param_ok = tolua_isusertype(L, 1, "FtkWidget", 0, &err) && tolua_isnumber(L, 2, 0, &err) && tolua_isnumber(L, 3, 0, &err) && tolua_isnumber(L, 4, 0, &err) && tolua_isnumber(L, 5, 0, &err);

	return_val_if_fail(param_ok, 0);

	parent = tolua_tousertype(L, 1, 0);
	x = tolua_tonumber(L, 2, 0);
	y = tolua_tonumber(L, 3, 0);
	width = tolua_tonumber(L, 4, 0);
	height = tolua_tonumber(L, 5, 0);
	retv = ftk_combo_box_create(parent, x, y, width, height);
	tolua_pushusertype(L, (FtkComboBox*)retv, "FtkComboBox");

	return 1;
}

static int lua_ftk_combo_box_get_text(lua_State* L)
{
	tolua_Error err = {0};
	char* retv;
	FtkWidget* thiz;
	int param_ok = tolua_isusertype(L, 1, "FtkWidget", 0, &err);

	return_val_if_fail(param_ok, 0);

	thiz = tolua_tousertype(L, 1, 0);
	retv = ftk_combo_box_get_text(thiz);
	tolua_pushstring(L, (char*)retv);

	return 1;
}

static int lua_ftk_combo_box_set_text(lua_State* L)
{
	tolua_Error err = {0};
	Ret retv;
	FtkWidget* thiz;
	char* text;
	int param_ok = tolua_isusertype(L, 1, "FtkWidget", 0, &err) && tolua_isstring(L, 2, 0, &err);

	return_val_if_fail(param_ok, 0);

	thiz = tolua_tousertype(L, 1, 0);
	text = (char*)tolua_tostring(L, 2, 0);
	retv = ftk_combo_box_set_text(thiz, text);
	tolua_pushnumber(L, (lua_Number)retv);

	return 1;
}

static int lua_ftk_combo_box_append(lua_State* L)
{
	tolua_Error err = {0};
	Ret retv;
	FtkWidget* thiz;
	FtkBitmap* icon;
	char* text;
	int param_ok = tolua_isusertype(L, 1, "FtkWidget", 0, &err) && tolua_isusertype(L, 2, "FtkBitmap", 0, &err) && tolua_isstring(L, 3, 0, &err);

	return_val_if_fail(param_ok, 0);

	thiz = tolua_tousertype(L, 1, 0);
	icon = tolua_tousertype(L, 2, 0);
	text = (char*)tolua_tostring(L, 3, 0);
	retv = ftk_combo_box_append(thiz, icon, text);
	tolua_pushnumber(L, (lua_Number)retv);

	return 1;
}

int tolua_ftk_combo_box_init(lua_State* L)
{
	tolua_open(L);
	tolua_reg_types(L);
	tolua_module(L, NULL, 0);
	tolua_beginmodule(L, NULL);
	tolua_cclass(L,"FtkComboBox", "FtkComboBox", "FtkWidget", NULL);
	tolua_beginmodule(L, "FtkComboBox");
	tolua_function(L, "Create", lua_ftk_combo_box_create);
	tolua_function(L, "GetText", lua_ftk_combo_box_get_text);
	tolua_function(L, "SetText", lua_ftk_combo_box_set_text);
	tolua_function(L, "Append", lua_ftk_combo_box_append);
	tolua_endmodule(L);
	tolua_endmodule(L);


	return 1;
}