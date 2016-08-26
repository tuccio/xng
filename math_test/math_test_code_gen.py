def write_template_function(f, funcName, templates, arguments):
	f.write(funcName + '<' + ', '.join(templates) + '>(' + ', '.join(arguments) + ');\n')

def generate_eigen_batch_multiply(type, min, max, iterations, tolerance):
	
	with open('test_batch_eigen_multiply.inl', 'w') as f:
	
		for i in range(min, max):
			for j in range(min, max):
				for k in range(min, max):
					write_template_function(f, 'test_batch_eigen_multiply', [str(type), str(i), str(j), str(k)], [str(iterations), str(tolerance)])
			
			
def generate_eigen_batch_add(type, min, max, iterations, tolerance):
	
	with open('test_batch_eigen_add.inl', 'w') as f:
	
		for i in range(min, max):
			for j in range(min, max):
				write_template_function(f, 'test_batch_eigen_add', [str(type), str(i), str(j)], [str(iterations), str(tolerance)])
				
def generate_eigen_batch_sub(type, min, max, iterations, tolerance):
	
	with open('test_batch_eigen_sub.inl', 'w') as f:
	
		for i in range(min, max):
			for j in range(min, max):
				write_template_function(f, 'test_batch_eigen_sub', [str(type), str(i), str(j)], [str(iterations), str(tolerance)])
				
def generate_eigen_batch_dot(type, min, max, iterations, tolerance):
	
	with open('test_batch_eigen_dot.inl', 'w') as f:
	
		for i in range(min, max):
			write_template_function(f, 'test_batch_eigen_dot', [str(type), '1', str(i)], [str(iterations), str(tolerance)])
			write_template_function(f, 'test_batch_eigen_dot', [str(type), str(i), '1'], [str(iterations), str(tolerance)])
			
def generate_eigen_batch_length(type, min, max, iterations, tolerance):
	
	with open('test_batch_eigen_length.inl', 'w') as f:
	
		for i in range(min, max):
			write_template_function(f, 'test_batch_eigen_length', [str(type), '1', str(i)], [str(iterations), str(tolerance)])
			write_template_function(f, 'test_batch_eigen_length', [str(type), str(i), '1'], [str(iterations), str(tolerance)])
			
def generate_eigen_batch_transpose(type, min, max, iterations, tolerance):
	
	with open('test_batch_eigen_transpose.inl', 'w') as f:
	
		for i in range(min, max):
			for j in range(min, max):
				write_template_function(f, 'test_batch_eigen_transpose', [str(type), str(i), str(j)], [str(iterations), str(tolerance)])
			

if __name__ == '__main__':

	generate_eigen_batch_transpose('float', 1, 5, 'Iterations', 1e-05)
	generate_eigen_batch_multiply('float', 1, 5, 'Iterations', 1e-05)
	generate_eigen_batch_add('float', 1, 5, 'Iterations', 1e-05)
	generate_eigen_batch_sub('float', 1, 5, 'Iterations', 1e-05)
	generate_eigen_batch_dot('float', 1, 5, 'Iterations', 1e-05)
	generate_eigen_batch_length('float', 1, 5, 'Iterations', 1e-02)