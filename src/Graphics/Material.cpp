#include "Material.h"

void Material::SetProperty(MaterialProperties property)
{
    properties = static_cast<MaterialProperties>(property | properties);
}
