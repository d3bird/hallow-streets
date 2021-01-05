#include "camera.h"


void Camera::print_stats() {
    std::cout << "pos: " << Position.x << " " << Position.y << " " << Position.z << std::endl;
    std::cout << "front: " << Front.x << " " << Front.y << " " << Front.z << std::endl;
    std::cout << "yaw: " << Yaw << std::endl;
    std::cout << "pitch: " << Pitch << std::endl;
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix(){
    return glm::lookAt(Position, Position + Front, Up);
}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime){
    float velocity = MovementSpeed * deltaTime;
    if (free_cam) {
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }
    else {
        velocity *= 2;
        if (direction == FORWARD)
            Position.z -=  velocity;
        if (direction == BACKWARD)
            Position.z +=  velocity;
        if (direction == LEFT)
            Position.x -=  velocity;
        if (direction == RIGHT)
            Position.x +=  velocity;
    }

    if (direction == UP)
        Position.y += velocity;
    if (direction == DOWN)
        Position.y -= velocity;
}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch){
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
float Camera::ProcessMouseScroll(float yoffset){
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
    
    //std::cout << "zoom = " << Zoom << std::endl;
    return Zoom;
}

    // calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors(){
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}