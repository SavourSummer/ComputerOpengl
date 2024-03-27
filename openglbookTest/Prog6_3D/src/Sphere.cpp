#include"Sphere.h"
#include<iostream>

Sphere::Sphere()
{
	init(48);//������캯���У�������init(48)��������ʼ�����塣�����48������ľ��ȣ���ʾ����ı��汻���ֳɶ��ٷݡ�����Խ�ߣ�����ı��滮�ֵ�Խϸ�����忴������Խƽ����
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
	/*��
	��Ϊ�ڴ�������Ĺ����У�����ͨ���Ὣ����ı��滮�ֳ�һϵ�е�С�����Σ�ÿ��С��������������������ɡ�������ǽ�����ı��濴��һ��ƽ������
	��ô�����������������������ھ���prec��
	
	��Ϊ������ͼ��ȫ��
����������Χ��������������ͼ�����ұ�Ե�ཻ��ÿ���㴦��Ҫһ������Ķ�����ʹ��ͼ����
�غϡ���ˣ������������(prec+1)*(prec+1)������ÿ���������� 6 �����������������������
������ prec*prec*6*/
	numVertices = (prec + 1) * (prec + 1);
	//��Ϊÿ���涼��������������ɣ�ÿ����������3�����㣬����ÿ������6��������
	numIndices = prec * prec * 6;//�������������ھ��ȵ�ƽ������6��������Ϊÿ���涼��������������ɣ�ÿ����������3�����㣬����ÿ������6������
	
	
	// std::vector::push_back() ��������ĩβ����һ����Ԫ�أ������������ȼ� 1 
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
		normals.push_back(glm::vec3());//�洢ÿ������ķ���������
	}
	for (int i = 0; i < numVertices; i++)
	{
		tangents.push_back(glm::vec3());
	}
	for (int i = 0; i < numIndices; i++) { indices.push_back(0); }//��indices���������numIndices��0����Щ0�����滻Ϊʵ�ʵ�����ֵ
	
	//  ���������ζ���
	/*y ��ͨ��������ϵ��γ�ȣ��ӱ������ϼ�������õ��ġ�
	x �� z ��ͨ��������ϵ�ľ��ȣ��س���ߣ���γ�ȼ���õ��ġ�
	vertices[i*(prec + 1) + j] �洢��ÿ����������ꡣ
	texCoords[i*(prec + 1) + j] �洢��ÿ��������������ꡣ
	normals[i*(prec + 1) + j] �洢��ÿ������ķ���������
	tangents[i*(prec + 1) + j] �洢��ÿ�����������������
	��������ڱ������ϼ��������������� (0,0,-1)������������������ (0,1,0) �Ͷ�������Ĳ����*/
	for (int i = 0; i <= prec; i++)
	{
		for (int j = 0; j <= prec; j++)
		{
			float y = (float)cos(toRadians(180.0f - i * 180.0f / prec));
			float x = (float)cos(toRadians(j * 360.0f / prec)) * (float)abs(cos(asin(y)));
			float z = (float)sin(toRadians(j * 360.0f / (float)(prec))) * (float)abs(cos(asin(y)));
			vertices[i * (prec + 1) + j] = glm::vec3(x, y, z);
			texCoords[i * (prec + 1) + j] = glm::vec2((float)j / prec, (float)i / prec);//�������U�����V���꣬���ǵķ�Χ����0��1֮��
			normals[i * (prec + 1) + j] = glm::vec3(x, y, z);
			// calculate tangent vector
			/*��������ڱ������ϼ����� (x, y, z) Ϊ (0, 1, 0) �� (0, -1, 0)����
			��ô������������Ϊ (0.0f, 0.0f, -1.0f)��
			����������������Ϊ (0.0f, 1.0f, 0.0f) 
			�Ͷ�������Ĳ������ glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z))��*/
			if (((x == 0) && (y == 1) && (z == 0)) || ((x == 0) && (y == -1) && (z == 0))) {
				tangents[i * (prec + 1) + j] = glm::vec3(0.0f, 0.0f, -1.0f);
			}
			else {
				tangents[i * (prec + 1) + j] = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z));
			}
		}
	}
	// ��������������
	for (int i = 0; i < prec; i++)
	{
		for (int j = 0; j < prec; j++)
		{	//�����εĵ�һ��������
			indices[6 * (i * prec + j) + 0] = i * (prec + 1) + j;
			indices[6 * (i * prec + j) + 1] = i * (prec + 1) + j + 1;
			indices[6 * (i * prec + j) + 2] = (i + 1) * (prec + 1) + j;
			//�����εĵڶ���������
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




