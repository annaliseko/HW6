#include "material.h"
#include "parse.h"
#include "image.h"
#include "main.h"
#include "rayfile.h"
#include "perlin.h"
#include <iostream>

const Color3d Material::getDiffuse(Intersection& info)
{
    std::cout << "i die in here" << std::endl;
    if (!textured() && !procTextured())
        std::cout << "not textured" << std::endl;
        return diffuse;
    if (textured())
        std::cout << "textured" << std::endl;
        return getTexture(info.texCoordinate[0], info.texCoordinate[1]);
    std::cout << "else" << std::endl;
    return getProceduralTexture(info.iCoordinate);
}

Material::Material ()
{
    name = texname = "";

    texture=NULL;
    bumpMap=NULL;

    pNoise[0]=pNoise[1]=pNoise[2]=NULL;
    // What are persistence and octaves and what are they used for?
    pOctaves=3;
    pPersistence=.5;
    glTexID = 0;
}


Material::~Material ()
{
    if (texture)
    {
        delete texture;
        glDeleteTextures(1, &glTexID);
    }
    if (bumpMap)
        delete bumpMap;
}


bool Material::textured ()
{
    std::cout << "why am i here" << std::endl;
    return false;
}

bool Material::bumpMapped ()
{
    return (bumpMap != NULL);
}

bool Material::procTextured()
{ 
    return false;
    std::cout << "procTextured" << std::endl;
    if (pNoise[0]==NULL)
        return false;
    std::cout << "aoiwefh;" << std::endl;
    return true;
}

const Color3d Material::getTexture(double u, double v)
{


    return Color3d(0,0,0);
}
const Color3d Material::getProceduralTexture(Point3d point)
{


    return Color3d(0,0,0);

}
void Material::bumpNormal (Vector3d& normal, 
                           Vector3d& up,
                           Vector3d& right,
                           Intersection& info,
                           double bumpScale)
{
    if (!options->bumpMaps || !bumpMapped())
        return;

    if (bumpMap != NULL)
    {

    }
    return;
}

void Material::glLoad ()
{
    /*
   * load the material's properties into OpenGL...turn on
   * texturing if necessary.
   */

    // if lighting is on, set material properties
    if (options->lighting)
    {
        // unfortunately, opengl needs the args in this format
        GLfloat vals[4][4] = { { static_cast<GLfloat>(ambient[0]),  static_cast<GLfloat>(ambient[1]),  static_cast<GLfloat>(ambient[2]),  static_cast<GLfloat>(ktrans) },
                               { static_cast<GLfloat>(diffuse[0]),  static_cast<GLfloat>(diffuse[1]),  static_cast<GLfloat>(diffuse[2]),  static_cast<GLfloat>(ktrans) },
                               { static_cast<GLfloat>(specular[0]), static_cast<GLfloat>(specular[1]), static_cast<GLfloat>(specular[2]), static_cast<GLfloat>(ktrans) },
                               { static_cast<GLfloat>(emissive[0]), static_cast<GLfloat>(emissive[1]), static_cast<GLfloat>(emissive[2]), static_cast<GLfloat>(ktrans) }
                             };

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   vals[0]);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   vals[1]);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  vals[2]);
        // What is emissive lighting? What does it do/how does it affect the object?
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  vals[3]);
        glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, kshine );
    }
    // if lighting is off, just set the color
    else
    {
        (ambient * scene->getAmbient() + emissive).glLoad();
    }

    // separate the specular light out for texture calculations
    if (texture && options->textures)
    {
#ifdef GL_VERSION_1_2
        glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
#endif

        glBindTexture(GL_TEXTURE_2D, glTexID);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
}


void Material::glUnload ()
{
    // go back to normal single color mode
    if (texture && options->textures)
    {
#ifdef GL_VERSION_1_2
        glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR);
#endif

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
}


ostream& operator<< (ostream& out, Material& m)
{
    return m.write(out);
}


ostream& Material::write (ostream& out)
{
    out << "#material -n " << name << flush;
    if (texture)
        out << " -t " << texname << flush;
    out << " --" << endl;

    out << "  " << ambient << endl;
    out << "  " << diffuse << endl;
    out << "  " << specular << endl;
    out << "  " << emissive << endl;
    out << "  " << kshine << " " << ktrans << " " << refind << endl;

    return out;
}


istream& operator>> (istream& in, Material& m)
{
    return m.read(in);
}


istream& Material::read (istream& in)
{
    bool pNoiseTrue=false;

    int numFlags = 6;
    char namebuf[MAX_NAME];
    char texbuf[MAX_PATH];
    char bumpbuf[MAX_PATH];
    Flag flags[6] = { { (char*)"-n", STRING, true,  MAX_NAME, namebuf },
                      { (char*)"-t", STRING, false, MAX_PATH, texbuf  },
                      { (char*)"-u", STRING, false, MAX_PATH, bumpbuf },
                      { (char*)"-p", BOOL,   false, sizeof(bool), &pNoiseTrue },
                      { (char*)"-po", INT, false, sizeof(int), &pOctaves},
                      { (char*)"-pp", DOUBLE, false, sizeof(double), &pPersistence}
                    };

    if (parseFlags(in, flags, numFlags) == -1)
    {
        cerr << "ERROR: Material: flag parsing failed!" << endl;
        return in;
    }

    in >> ambient >> diffuse >> specular >> emissive;

    kshine  = nextDouble(in);
    if (kshine < 0 || kshine>1)
    {
        cerr << "WARNING: Invalid kshine value clamped to [0,1]." << endl;
    }
    kshine = clamp(kshine,0,1);
    // now we scale kshine to range [0,128]
    kshine *= 128.0;

    ktrans = nextDouble(in);
    refind = nextDouble(in);
    if (refind<1)
    {
        cerr << "WARNING: Invalid refractive index (refind) reset to 1." << endl;
        refind=1;
    }

    if (in.fail())
    {
        cerr << "ERROR: Material: unknown stream failure" << endl;
        return in;
    }

    if (texbuf[0] != '\0')
    {
        if (texture)
            delete texture;

        char buf[MAX_PATH];
        strncat(buf, "/", 2);
        strncat(buf, texbuf, MAX_PATH);
        texture = new Image(buf);
        texname = string(buf);

        if(texture->good())
        {
            glGenTextures(1, &glTexID);
            glBindTexture(GL_TEXTURE_2D, glTexID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            texture->glTexImage2DWrapper();
        }
        else
        {
            cerr << "WARNING: Unable to load texture for material: " << name;
            cerr << endl;
            delete texture;
            texture = NULL;
        }
    }
    if (bumpbuf[0] != '\0')
    {
        if (bumpMap)
            delete bumpMap;

        char buf[MAX_PATH];
        strncat(buf, "/", 2);
        strncat(buf, bumpbuf, MAX_PATH);
        bumpMap = new Image(buf);
        bumpname = string(buf);

        if(!bumpMap->good())
        {
            cerr << "WARNING: Unable to load bump map for material: "
                 << name << endl;
            delete bumpMap;
            bumpMap = NULL;
        }
    }

    if (pNoiseTrue) {
        pNoise[0] = new PerlinNoise(137777893); // for now hard code seed
        pNoise[1] = new PerlinNoise(9973281);
        pNoise[2] = new PerlinNoise(399990001);
    }

    name = string(namebuf);

    return in;
}


void Material::setMatDirectory (const char* dir)
{
    texname = bumpname = "";

    char buf[MAX_PATH];

    if (dir == NULL || dir[0] == '\0')
        texname = ".";
    else
        texname = dir;

    if (dir == NULL || dir[0] == '\0')
        bumpname = ".";
    else
        bumpname = dir;
}
