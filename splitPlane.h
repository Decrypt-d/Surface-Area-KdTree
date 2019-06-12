#pragma once

class splitPlane
{
	public:
		int pk;		
		float pe;
		splitPlane(int pk, float pe) : pe(pe), pk(pk){}
		bool operator==(const splitPlane & second) 
		{
			if (second.pk == pk && second.pe == pe)
				return true;
			return false;
		}
};
