#include"Sphere.h"
#include<iostream>

Sphere::Sphere()
{
	init(48);//这个构造函数中，调用了init(48)函数来初始化球体。这里的48是球体的精度，表示球体的表面被划分成多少份。精度越高，球体的表面划分得越细，球体看起来就越平滑。
}

Sphere::Sphere(int prec)
{
	init(prec);
}

float Sphere::toRadians(float degrees)
{
	return (degrees * 2.0f * 3.14159f) / 360.0f;
}

void Sphere::init(int prec)
{
	/*。
	因为在创建球体的过程中，我们通常会将球体的表面划分成一系列的小正方形，每个小正方形由两个三角形组成。如果我们将球体的表面看作一个平面网格，
	那么这个网格的行数和列数都等于精度prec。
	
	因为纹理贴图完全包
裹在球体周围，所以在纹理贴图的左右边缘相交的每个点处需要一个额外的顶点来使贴图两侧
重合。因此，顶点的总数是(prec+1)*(prec+1)。由于每个顶点生成 6 个三角形索引，因此索引的
总数是 prec*prec*6*/
	numVertices = (prec + 1) * (prec + 1);
	//因为每个面都由两个三角形组成，每个三角形有3个顶点，所以每个面有6个索引。
	numIndices = prec * prec * 6;//索引的数量等于精度的平方乘以6。这是因为每个面都由两个三角形组成，每个三角形有3个顶点，所以每个面有6个索引
	
	
	// std::vector::push_back() 在向量的末尾增加一个新元素，并将向量长度加 1 
	for (int i = 0; i < numVertices; i++)
	{
		vertices.push_back(glm::vec3());
	}
	for (int i = 0; i < numVertices;i++)
	{
		texCoords.push_back(glm::vec2());
	}
	for (int i = 0; i < numVertices; i++)
	{
		normals.push_back(glm::vec3());//存储每个顶点的法线向量。
	}
	for (int i = 0; i < numVertices; i++)
	{
		tangents.push_back(glm::vec3());
	}
	for (int i = 0; i < numIndices; i++) { indices.push_back(0); }//在indices向量中添加numIndices个0。这些0将被替换为实际的索引值
	
	//  计算三角形顶点
	/*y 是通过球坐标系的纬度（从北极到南极）计算得到的。
	x 和 z 是通过球坐标系的经度（沿赤道线）和纬度计算得到的。
	vertices[i*(prec + 1) + j] 存储了每个顶点的坐标。
	texCoords[i*(prec + 1) + j] 存储了每个顶点的纹理坐标。
	normals[i*(prec + 1) + j] 存储了每个顶点的法线向量。
	tangents[i*(prec + 1) + j] 存储了每个顶点的切线向量。
	如果顶点在北极或南极，切线向量就是 (0,0,-1)，否则，切线向量就是 (0,1,0) 和顶点坐标的叉积。*/
	for (int i = 0; i <= prec; i++)
	{
		for (int j = 0; j <= prec; j++)
		{
			float y = (float)cos(toRadians(180.0f - i * 180.0f / prec));
			float x = (float)cos(toRadians(j * 360.0f / prec)) * (float)abs(cos(asin(y)));
			float z = (float)sin(toRadians(j * 360.0f / (float)(prec))) * (float)abs(cos(asin(y)));
			vertices[i * (prec + 1) + j] = glm::vec3(x, y, z);
			texCoords[i * (prec + 1) + j] = glm::vec2((float)j / prec, (float)i / prec);//计算出了U坐标和V坐标，它们的范围都在0到1之间
			normals[i * (prec + 1) + j] = glm::vec3(x, y, z);
			// calculate tangent vector
			/*如果顶点在北极或南极（即 (x, y, z) 为 (0, 1, 0) 或 (0, -1, 0)），
			那么切线向量就设为 (0.0f, 0.0f, -1.0f)。
			否则，切线向量就设为 (0.0f, 1.0f, 0.0f) 
			和顶点坐标的叉积，即 glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z))。*/
			if (((x == 0) && (y == 1) && (z == 0)) || ((x == 0) && (y == -1) && (z == 0))) {
				tangents[i * (prec + 1) + j] = glm::vec3(0.0f, 0.0f, -1.0f);
			}
			else {
				tangents[i * (prec + 1) + j] = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z));
			}
		}
	}
	// 计算三角形索引
	for (int i = 0; i < prec; i++)
	{
		for (int j = 0; j < prec; j++)
		{	//正方形的第一个三角形
			indices[6 * (i * prec + j) + 0] = i * (prec + 1) + j;
			indices[6 * (i * prec + j) + 1] = i * (prec + 1) + j + 1;
			indices[6 * (i * prec + j) + 2] = (i + 1) * (prec + 1) + j;
			//正方形的第二个三角形
			indices[6 * (i * prec + j) + 3] = i * (prec + 1) + j + 1;
			indices[6 * (i * prec + j) + 4] = (i + 1) * (prec + 1) + j + 1;
			indices[6 * (i * prec + j) + 5] = (i + 1) * (prec + 1) + j;
		}
	}
}

int Sphere::getNumVertices()
{
	return numVertices;
}
int Sphere::getNumIndices()
{
	return numIndices;
}
std::vector<int> Sphere::getIndices() { return indices; }
std::vector<glm::vec3> Sphere::getVertices() { return vertices; }
std::vector<glm::vec2> Sphere::getTexCoords() { return texCoords; }
std::vector<glm::vec3> Sphere::getNormals() { return normals; }
std::vector<glm::vec3> Sphere::getTangents() { return tangents; }




