#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <iomanip>

#include <GLFW/glfw3.h> // The GLFW header
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtx/normal.hpp>

namespace vcu {
	struct Vertex
	{
		Vertex() : x(0), y(0), z(0) {}
		Vertex(float inX, float inY, float inZ) : x(inX), y(inY), z(inZ) { }


		Vertex operator+(Vertex& v)
		{
			return Vertex(x + v.x, y + v.y, z + v.z);
		}
		Vertex operator*(float& f)
		{
			return Vertex(x * f, y * f, z * f);
		}
		Vertex operator*(Vertex v)
		{
			return Vertex(x * v.x, y * v.y, z * v.z);
		}

		glm::vec3 vertexToGlmVec3()
		{
			return glm::vec3(x, y, z);
		}
		float x, y, z;
	};

	struct Texture
	{
		Texture(float inU, float inV) : u(inU), v(inV) { }
		float u, v;
	};

	struct Normal
	{
		Normal() : x(0), y(0), z(0) {}
		//Normal(Normal n) : x(n.x), y(n.y), z(n.z){}
		Normal(float inX, float inY, float inZ) : x(inX), y(inY), z(inZ) { }
		Normal(glm::vec3& vec) : x(vec.x), y(vec.y), z(vec.z) { }

		glm::vec3 normalToGlmVec3()
		{
			return glm::vec3(x, y, z);
		}


		Normal& operator+=(Normal& n)
		{
			x += n.x;
			y += n.y;
			z += n.z;
			return *this;
		}

		float x, y, z;
	};

	struct Face
	{
		Face(int v[], int t[], int n[]) {
			vIndex[0] = v[0];
			vIndex[1] = v[1];
			vIndex[2] = v[2];
			tIndex[0] = t[0];
			tIndex[1] = t[1];
			tIndex[2] = t[2];
			nIndex[0] = n[0];
			nIndex[1] = n[1];
			nIndex[2] = n[2];
		}
		uint32_t vIndex[3], tIndex[3], nIndex[3];
	};

	std::vector<Vertex> gVertices;
	std::vector<Texture> gTextures;
	std::vector<Normal> gNormals;
	std::vector<Face> gFaces;

	GLuint gVertexAttribBuffer, gIndexBuffer;
	GLint gInVertexLoc, gInNormalLoc;
	int gVertexDataSizeInBytes, gNormalDataSizeInBytes;

	// new variables
	int nSample = 10;
	static float rotationAngle = -30.0f;
	float coordMultiplier = 1.0;
	// Store control points

	struct Patch
	{
		Vertex patchBezierControlPoints[4][4]; // 4x4 = 16 CPs
	};

	int verticalCPCount = 4, horizontalCPCount = 4;

	std::vector<Patch> bezierPatches;

	std::vector<std::vector<float>> heightOfCPs;

	std::vector<Vertex> bezierSampleVertices;
	std::vector<Vertex> bezierNormalVertices;

	bool parseInputFile(const std::string fileName)
	{
		std::fstream inputFile;
		// TODO: add assert for not reading a file
		inputFile.open(fileName.c_str());

		std::cerr << fileName + " file is read" << std::endl;

		inputFile >> verticalCPCount;
		inputFile >> horizontalCPCount;

		std::cerr << "vertical CP Count " + verticalCPCount << std::endl;
		std::cerr << "horizontal CP Count " + horizontalCPCount << std::endl;

		int numberOfPatch = ((verticalCPCount / 4) * (horizontalCPCount / 4));
		std::cerr << "Number of Patch " + numberOfPatch << std::endl;

		bezierPatches.resize(numberOfPatch);

		// get all height values of CPs
		for (int v = 0; v < verticalCPCount; v++)
		{
			std::vector<float> height;
			for (int h = 0; h < horizontalCPCount; h++)
			{
				float fHeight;
				inputFile >> fHeight;
				height.push_back(fHeight);
				//height.push_back(0);

			}
			heightOfCPs.push_back(height);

		}

		// get all z values and put them into bezierSurfaceVertices
		return true;
	}

	std::vector<float> linSpace(float start, float end, int sampleNumber)
	{
		std::vector<float> result;

		for (int i = 0; i < sampleNumber; i++)
		{
			float lineDiv = start + i * (end - start) / (float)(sampleNumber - 1);
			result.push_back(lineDiv);
		}
		return result;
	}

	int factorial(int n)
	{
		// @TODO I found more efficient way on the internet, will be optimized later
		if (n < 2)
			return 1;
		return n * factorial(n - 1);
	}

	int nChoose(int n, int i)
	{
		return factorial(n) / factorial(i) / factorial(n - i);
	}

	float bernstein(int i, int n, float t)
	{
		return nChoose(n, i) * pow(t, i) * pow(1 - t, n - i);

	}

	float bernsteinDerivative(int i, int n, float t)
	{
		float ret = nChoose(n, i) * pow(t, (i - 1)) * pow(1 - t, (n - i - 1)) * (-(n - i) * t + i * (1 - t));
		return (isnan(ret)) ? 1 : ret;
	}

	Vertex Q(float s, float t, Patch bezierPatch)
	{
		Vertex tempVertex;
		Vertex tempnormalVert;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				tempVertex.x += bernstein(i, 3, s) * bernstein(j, 3, t) * bezierPatch.patchBezierControlPoints[i][j].x;
				tempVertex.y += bernstein(i, 3, s) * bernstein(j, 3, t) * bezierPatch.patchBezierControlPoints[i][j].y;
				tempVertex.z += bernstein(i, 3, s) * bernstein(j, 3, t) * bezierPatch.patchBezierControlPoints[i][j].z;

			}
		}

		return tempVertex;
	}

	Vertex Qder(float s, float t, Patch bezierPatch)
	{
		Vertex tt;
		Vertex ss;
		Vertex tempnormalVert;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				ss.x += bernsteinDerivative(i, 3, s) * bezierPatch.patchBezierControlPoints[i][j].x;
				ss.y += bernsteinDerivative(i, 3, s) * bezierPatch.patchBezierControlPoints[i][j].y;
				ss.z += bernsteinDerivative(i, 3, s) * bezierPatch.patchBezierControlPoints[i][j].z;

				tt.x += bernsteinDerivative(j, 3, t) * bezierPatch.patchBezierControlPoints[i][j].x;
				tt.y += bernsteinDerivative(j, 3, t) * bezierPatch.patchBezierControlPoints[i][j].y;
				tt.z += bernsteinDerivative(j, 3, t) * bezierPatch.patchBezierControlPoints[i][j].z;

			}

			glm::vec3 surfaceNormal = glm::vec3(tt.y * ss.z - tt.z * ss.y, tt.z * ss.x - tt.x * ss.z, tt.x * ss.y - tt.y * ss.x);
			tempnormalVert.x = surfaceNormal.x;
			tempnormalVert.y = surfaceNormal.y;
			tempnormalVert.z = surfaceNormal.z;
		}

		return tempnormalVert;
	}

	void createControlPoints()
	{
		int verticalPatch = verticalCPCount / 4;
		int horizontalPatch = horizontalCPCount / 4;
		int patchSize = bezierPatches.size();
		int b = 0;
		std::vector<float> vLin;
		std::vector<float> hLin;

		{


			if (horizontalPatch > verticalPatch)
			{
				vLin = linSpace(0.5 * coordMultiplier, -0.5 * coordMultiplier, verticalCPCount);
				hLin = linSpace(-0.5 * 2 * coordMultiplier, 0.5 * 2 * coordMultiplier, horizontalCPCount);

			}
			else if (horizontalPatch < verticalPatch)
			{
				vLin = linSpace(0.5 * 2 * coordMultiplier, -0.5 * 2 * coordMultiplier, verticalCPCount);
				hLin = linSpace(-0.5 * coordMultiplier, 0.5 * coordMultiplier, horizontalCPCount);

			}
			else if (horizontalPatch == verticalPatch)
			{
				vLin = linSpace(0.5 * coordMultiplier, -0.5 * coordMultiplier, verticalCPCount);
				hLin = linSpace(-0.5 * coordMultiplier, 0.5 * coordMultiplier, horizontalCPCount);
			}
			float* xl = vLin.data();
			float* yl = hLin.data();

			for (int y = 0; y < verticalPatch; y++)
			{
				for (int x = 0; x < horizontalPatch; x++)
				{
					for (int i = 0; i < 4; i++)
					{
						for (int j = 0; j < 4; j++)
						{
							// x y z 

							bezierPatches[b].patchBezierControlPoints[i][j].x = *(yl + j + (x * 3)); // i was multiply with 4 then there is a gap between patches so i choose 3
							bezierPatches[b].patchBezierControlPoints[i][j].y = *(xl + i + (y * 3));
							bezierPatches[b].patchBezierControlPoints[i][j].z = heightOfCPs[i + (y * 4)][j + (x * 4)]; // 16

						}
					}
					b++;

				}
			}


		}

	}

	void initBezierSampleVertices()
	{
		std::vector<float> s = linSpace(0, 1, nSample);
		std::vector<float> t = linSpace(0, 1, nSample);

		for (int b = 0; b < bezierPatches.size(); b++)
		{
			for (int i = 0; i < s.size(); i++)
			{
				for (int j = 0; j < t.size(); j++)
				{
					bezierSampleVertices.push_back(Q(s[i], t[j], bezierPatches[b]));
				}
			}
		}
	}


	void generateBezierFaces() // + normals
	{
		gNormals.resize(nSample * nSample * bezierPatches.size());

		for (int b = 0; b < bezierPatches.size(); b++)
		{
			for (int i = 0; i < nSample - 1; i++)
			{
				for (int j = 0; j < nSample - 1; j++)
				{


					// //face normals
					glm::vec3 v1 = bezierSampleVertices[i * nSample + j + b * nSample * nSample].vertexToGlmVec3(); // 0
					glm::vec3 v2 = bezierSampleVertices[(i + 1) * nSample + j + b * nSample * nSample].vertexToGlmVec3(); // 4
					glm::vec3 v3 = bezierSampleVertices[(i * nSample) + j + 1 + b * nSample * nSample].vertexToGlmVec3(); // 1
					glm::vec3 v4 = bezierSampleVertices[(i + 1) * nSample + j + 1 + b * nSample * nSample].vertexToGlmVec3(); //5


					glm::vec3 n1 = glm::triangleNormal(v1, v3, v2) * glm::vec3(-1);
					glm::vec3 n2 = glm::triangleNormal(v2, v1, v3) * glm::vec3(-1);
					glm::vec3 n3 = glm::triangleNormal(v3, v2, v1) * glm::vec3(-1);
					//

					Normal cross;
					cross.x = (n1.x + n2.x + n3.x);
					cross.y = (n1.y + n2.y + n3.y);
					cross.z = (n1.z + n2.z + n3.z);

					glm::vec3 n4 = glm::triangleNormal(v3, v2, v4) * glm::vec3(-1);
					glm::vec3 n5 = glm::triangleNormal(v4, v2, v3) * glm::vec3(-1);
					glm::vec3 n6 = glm::triangleNormal(v2, v3, v4) * glm::vec3(-1);

					Normal cross2;
					cross2.x = (n4.x + n5.x + n6.x);
					cross2.y = (n4.y + n5.y + n6.y);
					cross2.z = (n4.z + n5.z + n6.z);

					gNormals[i * nSample + j + b * nSample * nSample] += cross;
					gNormals[(i + 1) * nSample + j + b * nSample * nSample] += cross;
					gNormals[i * nSample + j + 1 + b * nSample * nSample] += cross;

					gNormals[i * nSample + j + 1 + b * nSample * nSample] += cross2;
					gNormals[(i + 1) * nSample + j + b * nSample * nSample] += cross2;
					gNormals[(i + 1) * nSample + j + 1 + b * nSample * nSample] += cross2;

					// construct faces
					int vInd[3];
					vInd[0] = (i * nSample) + j + b * nSample * nSample; //0
					vInd[1] = (i + 1) * nSample + j + b * nSample * nSample; // 4
					vInd[2] = (i * nSample) + j + 1 + b * nSample * nSample; // 1
					gFaces.push_back(Face(vInd, vInd, vInd));

					vInd[0] = (i + 1) * nSample + j + b * nSample * nSample; // 4
					vInd[1] = (i + 1) * nSample + j + 1 + b * nSample * nSample; // 5
					vInd[2] = (i * nSample) + j + 1 + b * nSample * nSample; // 1
					gFaces.push_back(Face(vInd, vInd, vInd));

				}

			}

		}

	}

	void calculateBezierNormals()
	{
		// gNormals

		for (int i = 0; i < gFaces.size(); i += 3)
		{
			float v1 = bezierSampleVertices[gFaces[i].vIndex[0]].x;
			float v2 = bezierSampleVertices[gFaces[i].vIndex[1]].y;
			float v3 = bezierSampleVertices[gFaces[i].vIndex[2]].z;
		}
	}

	void changeSampleSize()
	{

		bezierPatches.clear();
		int numberOfPatch = ((verticalCPCount / 4) * (horizontalCPCount / 4));
		std::cerr << "Number of Patch " + numberOfPatch << std::endl;

		bezierPatches.resize(numberOfPatch);

		bezierSampleVertices.clear();
		gNormals.clear();
		gFaces.clear();

		createControlPoints();
		initBezierSampleVertices();
		generateBezierFaces();
		//initBuffers();
	}

}
