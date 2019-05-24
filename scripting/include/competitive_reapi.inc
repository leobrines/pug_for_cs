#include <reapi>

stock rg_remove_entity(index)
{
	set_entvar(index, var_flags, get_entvar(index, var_flags) | FL_KILLME);
	//set_entvar(index, var_nextthink, get_gametime() );
}

stock rg_set_rendering(ent, fx=kRenderFxNone, r=255, g=255, b=255, render=kRenderNormal, amount=16) 
{
	static Float:dest[3]
	dest[0] = float(r)
	dest[1] = float(g)
	dest[2] = float(b)

	set_entvar(ent, var_renderfx, fx)
	set_entvar(ent, var_rendercolor, dest)
	set_entvar(ent, var_rendermode, render)
	set_entvar(ent, var_renderamt, float(amount)) 
}  
