#include "commom.h"
#include "httprequest.h"
httprequest::httprequest(char *p)
{
	req=p;

}
int httprequest::parse()
{
   int r_pos=parseline(req);
   parsehead(req,r_pos);
   return 1;
   
}
httprequest::~httprequest(){}