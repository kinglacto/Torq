#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class cameraDirection {
	NONE = 0,
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera {
public:
	explicit Camera(glm::vec3 position);

	void updateCameraDirection(double dx, double dy);
	void updateCameraPos(cameraDirection dir, float dt);
	void updateCameraZoom(double dy);

	[[nodiscard]] float getZoom() const;

	[[nodiscard]] glm::mat4 getViewMatrix();

	void updateCameraVectors();

	float zoom;

	glm::vec3 cameraPos;

	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 cameraRight;

	glm::vec3 worldUp;

	double yaw;
	double pitch;
	double sensitivity;

	float znear = 1.0f;
	float zfar = 4000.0f;

	double deltaTime = 0;
	float speed;
};

#endif //CAMERA_H