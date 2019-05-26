#include <reapi>
#include <fakemeta>

new Float:color[3]

stock rg_remove_entity (const ent)
{
	if (!is_entity(ent))
		return;

	engfunc(EngFunc_RemoveEntity, ent);
}

stock rg_set_rendering (const ent, const fx=kRenderFxNone, r,g,b, render=kRenderNormal, amount=16) 
{
	if (!is_entity(ent))
		return;

	color[0] = float(r);
	color[1] = float(g);
	color[2] = float(b);

	set_entvar(ent, var_renderfx, fx)
	set_entvar(ent, var_rendercolor, color)
	set_entvar(ent, var_rendermode, render)
	set_entvar(ent, var_renderamt, float(amount)) 
}

public bool:is_user_playing (const id) {
	if ( is_user_alive(id) &&
		is_user_connected(id) &&
		(get_member(id, m_iTeam) != TEAM_SPECTATOR))
		return true;

	return false;
}
