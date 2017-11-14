#include "light.h"
#include <string.h>


Light::Light()
{
    name = "";
}


Light::~Light()
{
}


istream& operator>> (istream& in, Light& l)
{
    return l.read(in);
}


ostream& operator<< (ostream& out, Light& l)
{
    return l.write(out);
}


const string Light::getName()
{
    return name;
}
