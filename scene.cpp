#include "scene.h"
#include "game.h"
#include "asteroidhandler.h"
#include "projectilehandler.h"
#include <mylly/core/mylly.h>
#include <mylly/core/time.h>
#include <mylly/renderer/mesh.h>
#include <mylly/renderer/shader.h>
#include <mylly/scene/scene.h>
#include <mylly/scene/object.h>
#include <mylly/scene/camera.h>
#include <mylly/scene/sprite.h>
#include <mylly/scene/light.h>
#include <mylly/scene/emitter.h>
#include <mylly/audio/audiosystem.h>
#include <mylly/resources/resources.h>
#include <mylly/math/random.h>

// -------------------------------------------------------------------------------------------------

// A struct where we can define various level backgrounds with lighting setup that fits them.
struct LevelBackground {

	const char *spriteName;
	colour_t ambientLight;
	colour_t directionalLight;
	Vec2 lightDirections[2];
	float lightIntensities[2];
};

static const LevelBackground levelBackgrounds[] = {
	{ "space1", col(40, 70, 150), col(140, 200, 255), { Vec2(-10, 5), Vec2(5, -10) }, { 1.2f, 0.4f } }, // 0
	{ "bot", col(80, 100, 150), col(100, 200, 255), { Vec2(10, -5), Vec2(5, 5) }, { 1.0f, 0.7f } }, // 1
	{ "space2", col(140, 90, 50), col(240, 210, 180), { Vec2(-10, 8), Vec2(-5, 3) }, { 0.7f, 0.7f } }, // 2
	{ "bkg1_front", col(50, 50, 100), col(80, 130, 255), { Vec2(-10, -5), Vec2(5, -5) }, { 1.6f, 0.1f } }, // 3
	{ "space3", col(80, 100, 150), col(100, 200, 255), { Vec2(10, -5), Vec2(5, 5) }, { 1.0f, 0.7f } }, // 4
	{ "bkg3_left2", col(130, 50, 50), col(255, 130, 110), { Vec2(10, 8), Vec2(-5, -3) }, { 1.4f, 0.2f }  }, // 5
};

// -------------------------------------------------------------------------------------------------

Scene::Scene(void)
{
}

Scene::~Scene(void)
{
	delete m_asteroids;
	delete m_projectiles;

	obj_destroy(m_camera->parent);
	m_camera = nullptr;

	obj_destroy(m_spaceBackground);

	obj_destroy(m_directionalLights[0]->parent);
	obj_destroy(m_directionalLights[1]->parent);

	uint32_t lightIndex;

	arr_foreach_iter(m_lightFlashes, lightIndex) {
		obj_destroy(m_lightFlashes.items[lightIndex].light->parent);
	}

	arr_clear(m_lightFlashes);

	if (m_sceneRoot != nullptr) {

		mylly_set_scene(nullptr);
		scene_destroy(m_sceneRoot);
	}
}

void Scene::Create(Game *game)
{
	UNUSED(game);
	
	// Create a scene root.
	m_sceneRoot = scene_create();
	mylly_set_scene(m_sceneRoot);

	// Create a handler for the asteroids in the game.
	m_asteroids = new AsteroidHandler();
	m_projectiles = new ProjectileHandler();

	CreateCamera();
}

void Scene::SetBackground(uint32_t backgroundIndex)
{
	if (backgroundIndex >= LENGTH(levelBackgrounds)) {
		backgroundIndex %= LENGTH(levelBackgrounds);
	}

	m_backgroundIndex = backgroundIndex;

	SetupLighting();

	// Create the non-transparent background object first.
	// NOTE: See the comment at the end of rsys_render_scene() in rendersystem.c!
	m_spaceBackground = CreateCameraTexture(levelBackgrounds[backgroundIndex].spriteName);

	// Create a fader texture which covers the entire view of the camera.
	m_fader = CreateCameraTexture("black", false);
	obj_set_active(m_fader, false);

	// Clone the black sprite's shader so we can control its colour freely.
	m_fadeShader = shader_clone(m_fader->sprite->mesh->shader);
	mesh_set_shader(m_fader->sprite->mesh, m_fadeShader);
}

void Scene::Update(Game *game)
{
	if (IsShaking()) {
		ProcessShake();
	}

	// Process light flashes.
	float deltaTime = get_time().delta_time;
	uint32_t lightIndex;

	arr_foreach_reverse_iter(m_lightFlashes, lightIndex) {

		LightFlash &flash = m_lightFlashes.items[lightIndex];
		flash.elapsed += deltaTime;

		float t = flash.elapsed / flash.duration;

		if (t >= 1) {
			obj_destroy(flash.light->parent);
			arr_remove_at(m_lightFlashes, lightIndex);
		}
		else {
			light_set_intensity(flash.light, (1 - t) * flash.intensity);
		}
	}

	// Process fade after everything else because when the fade ends the scene can be deleted.
	if (IsFading()) {
		ProcessFade(game);
	}
}

void Scene::CalculateBoundaries(Vec2 &outMin, Vec2 &outMax)
{
	if (m_camera == nullptr) {

		outMin = Vec2(0, 0);
		outMax = Vec2(100, 100);
		return;
	}

	// Calculate game area boundaries with the current camera. OpenGL vertical coordinates
	// get smaller from top to botton, hence the reversed Y coordinate.
	uint16_t screenWidth, screenHeight;
	mylly_get_resolution(&screenWidth, &screenHeight);

	vec3_t min = camera_screen_to_world(m_camera, vec3(0, (float)screenHeight, 0.1f));
	vec3_t max = camera_screen_to_world(m_camera, vec3((float)screenWidth, 0, 0.1f));

	float padding = 2.0f;

	outMin = vec2(min.x - padding, min.z - padding);
	outMax = vec2(max.x + padding, max.z + padding);
}

void Scene::AddCameraEffect(shader_t *effect)
{
	if (effect != nullptr) {
		camera_add_post_processing_effect(m_camera, effect);
	}
}

void Scene::RemoveCameraEffect(shader_t *effect)
{
	if (effect != nullptr) {
		camera_remove_post_processing_effect(m_camera, effect);
	}
}

void Scene::FadeCamera(bool fadeIn)
{
	m_fadeFactor = (fadeIn ? 0 : 1);
	m_isFadingIn = fadeIn;
	m_fadeEffectEnds = get_time().real_time + FADE_DURATION;

	// Activate the fader object and set its starting colour.
	obj_set_active(m_fader, true);

	colour_t startColour = col_a(0, 0, 0, 255);

	if (!fadeIn) {
		startColour = col_a(0, 0, 0, 0);
	}

	shader_set_uniform_colour(m_fadeShader, "Colour", startColour);
}

void Scene::ShakeCamera(float intensity, float duration)
{
	m_shakeIntensity = intensity;
	m_shakeDuration = duration;
	m_shakeElapsed = 0;
}

emitter_t *Scene::SpawnEffect(const char *effectName, const Vec2 &position) const
{
	// Find the effect resource.
	emitter_t *effect = res_get_emitter(effectName);

	if (effect == nullptr) {
		return nullptr;
	}

	// Spawn the object and attach a particle emitter to it.
	object_t *effectObject = scene_create_object(m_sceneRoot, nullptr);
	emitter_t *emitter = obj_add_emitter(effectObject, effect);

	// Move the object to the desired position and rotate it towards the camera.
	obj_set_position(effectObject, vec3(position.x(), 0, position.y()));
	obj_set_local_rotation(effectObject, quat_from_euler_deg(90, 0, 0));

	// Set the emitter to be destroyed after it becomes inactive and start it.
	emitter_set_destroy_when_inactive(emitter, true);
	emitter_start(emitter);

	return emitter;
}

void Scene::SpawnLightFlash(const Vec2 &position, const colour_t &colour,
                     float intensity, float duration)
{
	// Create a light instance into the scene.
	object_t *object = scene_create_object(m_sceneRoot, nullptr);
	light_t *light = obj_add_light(object);

	obj_set_position(object, vec3(position.x(), 0, position.y()));

	light_set_type(light, LIGHT_POINT);
	light_set_colour(light, colour);
	light_set_intensity(light, intensity);
	light_set_range(light, 20);

	// Store the light to a list so it can be dimmed.
	LightFlash flash;

	flash.light = light;
	flash.intensity = intensity;
	flash.duration = duration;
	flash.elapsed = 0;

	arr_push(m_lightFlashes, flash);
}

void Scene::CreateCamera(void)
{
	// Create a camera object and add it to the scene.
	object_t *cameraObject = scene_create_object(m_sceneRoot, nullptr);
	m_camera = obj_add_camera(cameraObject);

	// Setup the camera's view.
	obj_set_position(cameraObject, vector3(0.0f, CAMERA_DEPTH, 0.0f));
	obj_look_at(cameraObject, vec3_zero(), vec3_forward());

	camera_set_orthographic_projection(m_camera, 45, ORTOGRAPHIC_NEAR, 100);

	// Apply anti-aliasing to the rendering result.
	camera_add_post_processing_effect(m_camera, res_get_shader("effect-fxaa"));

	// Make the camera the audio listener.
	audio_set_listener(cameraObject);
}

object_t *Scene::CreateCameraTexture(const char *spriteName, bool isBackground)
{
	// Create an object for the background element and attach a sprite to it.
	object_t *object = scene_create_object(m_sceneRoot, nullptr);
	obj_set_position(object, vec3(0, (isBackground ? 20.0f : -49.0f), 0));

	sprite_t *sprite = res_get_sprite(spriteName);
	obj_set_sprite(object, sprite);

	// Create a copy of the default sprite shader and make it draw in the background queue.
	if (isBackground) {

		shader_t *bg_shader = shader_clone(sprite->mesh->shader);
		shader_set_render_queue(bg_shader, QUEUE_BACKGROUND);

		sprite_set_shader(sprite, bg_shader);
	}

	// Get the size of the sprite in world units and scale it to cover the entire view of the camera.
	Vec2 min, max;
	CalculateBoundaries(min, max);

	float spriteWidth = sprite->size.x / sprite->pixels_per_unit;
	float viewWidth = max.x() - min.x();
	float scale = viewWidth / spriteWidth;

	obj_set_local_scale(object, vec3(scale, scale, 1));

	// Rotate the sprite to face the camera.
	obj_set_local_rotation(object, quat_from_euler_deg(90, 0, 0));

	return object;
}

void Scene::SetupLighting(void)
{
	const LevelBackground &background = levelBackgrounds[m_backgroundIndex];

	// Adjust ambient lighting.
	scene_set_ambient_light(m_sceneRoot, background.ambientLight);

	// Create two directional lights to light the ship and other 3D objects.
	for (int i = 0; i < 2; i++) {
		
		object_t *lightObject = scene_create_object(m_sceneRoot, nullptr);
		light_t *light = obj_add_light(lightObject);

		m_directionalLights[i] = light;

		light_set_type(light, LIGHT_DIRECTIONAL);
		light_set_colour(light, background.directionalLight);
		light_set_intensity(light, 0.5f * background.lightIntensities[i]);

		Vec2 direction = background.lightDirections[i];
		light_set_direction(light, vec3(direction.x(), -3, direction.y()));
	}
}

void Scene::ProcessFade(Game *game)
{
	float time = get_time().real_time;

	if (time >= m_fadeEffectEnds) {

		m_fadeFactor = (m_isFadingIn ? 0 : 1);
		m_fadeEffectEnds = 0;
		obj_set_active(m_fader, false);

		if (m_isFadingIn) {
			// When fading in, we're loading a new scene.
		}
		else {

			// When fading out, we're unloading an old scene. Tell the game to change to a new one.
			game->ChangeScene();
		}
		return;
	}

	float timeLeft = m_fadeEffectEnds - time;
	float t = 1.0f - (timeLeft / FADE_DURATION);

	m_fadeFactor = (m_isFadingIn ? t : 1.0f - t);

	colour_t start, end;

	if (m_isFadingIn) {
		start = col_a(0, 0, 0, 255);
		end = col_a(0, 0, 0, 0);
	}
	else {
		start = col_a(0, 0, 0, 0);
		end = col_a(0, 0, 0, 255);
	}

	colour_t fadeColour = col_lerp(start, end, t);
	shader_set_uniform_colour(m_fadeShader, "Colour", fadeColour);
}

void Scene::ProcessShake(void)
{
	if (m_camera == nullptr ||
		m_camera->parent == nullptr) {
		return;
	}
	
	// Apply the shake effect offset to the camera. The camera is always centered at the origin.
	Vec3 position = Vec3(0, CAMERA_DEPTH, 0);

	if (m_shakeElapsed < m_shakeDuration) {

		float t = 1.0f - m_shakeElapsed / m_shakeDuration;

		position = Vec3(
			m_shakeIntensity * expf(-2 * t) * cosf(randomf(0, 20)) * t,
			CAMERA_DEPTH,
			m_shakeIntensity * expf(-2 * t) * sinf(randomf(0, 20)) * t
		);
	}
	else {
		m_shakeDuration = 0;
	}

	obj_set_position(m_camera->parent, position.vec());

	m_shakeElapsed += get_time().real_delta_time;
}
