{
	version: 1,
	emitter: {
		sprite: "flash04",
		is_world_space: true,
		max_particles: 500,
		emit_duration: 0.000,
		emit_rate: 0.000,
		initial_burst: 10,
		life_min: 0.200,
		life_max: 0.200,
		start_size_min: 5.000,
		start_size_max: 5.000,
		end_size_min: 5.000,
		end_size_max: 5.000,
		rotation_min: 0.000,
		rotation_max: 0.000,
		start_speed_min: 2.000,
		start_speed_max: 3.000,
		end_speed_min: 2.000,
		end_speed_max: 3.000,
		acceleration_min: [ 0.000, 0.000, 0.000 ],
		acceleration_max: [ 0.000, 0.000, 0.000 ],
		start_colour_min: [ 255, 182, 140, 46 ],
		start_colour_max: [ 255, 217, 178, 36 ],
		end_colour_min: [ 44, 44, 44, 0 ],
		end_colour_max: [ 65, 65, 65, 0 ],
		emit_position: [ 0.000, 0.000, 0.000 ],
		emit_circle_radius: 0.000,
		subemitters: [
			{
				effect: "asteroid-smoke",
				type: 0,
			},
			{
				effect: "asteroid-debris",
				type: 0,
			},
		],
	},
}