#include "raytracer_renderer.h"

#include "utils/resource_utils.h"

#include <iostream>


void cg::renderer::ray_tracing_renderer::init()
{
	model = std::make_shared<cg::world::model>();
	model->load_obj(settings->model_path);

	camera = std::make_shared<cg::world::camera>();
	camera->set_height(static_cast<float>(settings->height));
	camera->set_width(static_cast<float>(settings->width));

	camera->set_position(float3{
			settings->camera_position[0],
			settings->camera_position[1],
			settings->camera_position[2]
	});
	camera->set_phi(settings->camera_phi);
	camera->set_theta(settings->camera_theta);
	camera->set_angle_of_view(settings->camera_angle_of_view);
	camera->set_z_near(settings->camera_z_near);
	camera->set_z_far(settings->camera_z_far);

	render_target = std::make_shared<cg::resource<cg::unsigned_color>>(settings->width, settings->height);

	raytracer = std::make_shared<cg::renderer::raytracer<cg::vertex, cg::unsigned_color>>();
	raytracer->set_render_target(render_target);
	raytracer->set_viewport(settings->width, settings->height);
	raytracer->set_vertex_buffers(model->get_vertex_buffers());
	raytracer->set_index_buffers(model->get_index_buffers());



	// TODO Lab: 2.03 Add light information to `lights` array of `ray_tracing_renderer`
	// TODO Lab: 2.04 Initialize `shadow_raytracer` in `ray_tracing_renderer`
}

void cg::renderer::ray_tracing_renderer::destroy() {}

void cg::renderer::ray_tracing_renderer::update() {}

void cg::renderer::ray_tracing_renderer::render()
{

	raytracer->clear_render_target({0,0,0});
	raytracer->miss_shader = [](const ray& ray){
		payload payload{};
		payload.color = {
				0.f,
				0.f,
				(ray.direction.y + 1.f) * 0.5f
		};
		return payload;
	};

	raytracer->closest_hit_shader = [](const ray& ray, payload& payload, const triangle<cg::vertex>& triangle, size_t depth){
		payload.color = cg::color::from_float3(triangle.diffuse);
		return payload;
	};

	raytracer->build_acceleration_structure();

	auto start = std::chrono::high_resolution_clock::now();
	raytracer->ray_generation(camera->get_position(),
							  camera->get_direction(),
							  camera->get_right(),
							  camera->get_up(),
							  settings->raytracing_depth,
							  settings->accumulation_num);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> raytracing_duration = end - start;
	std::cout << "Duration " << raytracing_duration.count() << "ms" << std::endl;
	cg::utils::save_resource(*render_target, settings->result_path);


	// TODO Lab: 2.03 Adjust `closest_hit_shader` of `raytracer` to implement Lambertian shading model
	// TODO Lab: 2.04 Define `any_hit_shader` and `miss_shader` for `shadow_raytracer`
	// TODO Lab: 2.04 Adjust `closest_hit_shader` of `raytracer` to cast shadows rays and to ignore occluded lights
	// TODO Lab: 2.05 Adjust `ray_tracing_renderer` class to build the acceleration structure
	// TODO Lab: 2.06 (Bonus) Adjust `closest_hit_shader` for Monte-Carlo light tracing
}