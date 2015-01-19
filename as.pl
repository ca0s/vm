#!usr/bin/perl

# "Ensamblador" para la máquina virtual churra de ca0s
# by el menda.

use strict;
use Scalar::Util 'looks_like_number';

# Valores para generar el ejecutable
my $code_size = 0;
my $data_size = 0;
my $code_offset = 35;	# magic + code_size + data_size + code_offset + data_offset
my $data_offset;

# Constantes de la implementación
use constant {
# Tiene argumentos?
	MASK_ARGS	=>	pack ("C", 0x80),	# 10000000
# 1 o 2 argumentos?
	MASK_NARGS	=>	pack ("C", 0x40),	# 01000000
# 32 o 64 bits?
	MASK_A1T	=>	pack ("C", 0x02),	# 00000010
	MASK_A2T	=>	pack ("C", 0x01),	# 00000001
	MASK_AT		=>	0,
# Dato o registro?
	MASK_A1M	=>	pack ("C", 0x08),	# 00001000
	MASK_A2M	=>	pack ("C", 0x04),	# 00000100
	MASK_AM		=>	1,
# Direccionamiento directo o indirecto?
	MASK_A1D	=>	pack ("C", 0x20),	# 00100000
	MASK_A2D	=>	pack ("C", 0x10),	# 00010000
	MASK_AD		=>	2
};

# Para mayor comodidad 
my @masks = (
		[ MASK_A1T, MASK_A1M, MASK_A1D ],
		[ MASK_A2T, MASK_A2M, MASK_A2D ]
);

# Descripción de opcodes. Ensamblado, argumentos, y opciones
my %opcodes = (
	ret		=>	{
					opcode	=>	pack ("C", 0x01),
					nargs	=>	0
				},
	call	=>	{
					opcode	=>	pack ("C", 0x02),
					nargs	=>	1,
					flags_1	=>	MASK_A1D | MASK_A1M | MASK_A1T
				},
	int		=>	{
					opcode	=>	pack ("C", 0x03),
					nargs	=>	1,
					flags_1	=>	MASK_A1D | MASK_A1M | MASK_A1T
				},
	jmp		=>	{
					opcode	=>	pack ("C", 0x04),
					nargs	=>	1,
					flags_1	=>	MASK_A1D | MASK_A1M | MASK_A1T
				},
	je		=>	{
					opcode	=>	pack ("C", 0x05),
					nargs	=>	1,
					flags_1	=>	MASK_A1D | MASK_A1M | MASK_A1T
				},
	z		=>	{
					opcode	=>	pack ("C", 0x06),
					nargs	=>	1,
					flags_1	=>	MASK_A1M | MASK_A1T
				},
	mv		=>	{
					opcode	=>	pack ("C", 0x07),
					nargs	=>	2,
					flags_1	=>	MASK_A1D | MASK_A1M | MASK_A1T,
					flags_2	=>	MASK_A2D | MASK_A2M | MASK_A2T
				},
	add		=>	{
					opcode	=>	pack ("C", 0x08),
					nargs	=>	2,
					flags_1	=>	MASK_A1M,
					flags_2	=>	MASK_A2D | MASK_A2M | MASK_A2T
				},
	sub		=>	{
					opcode	=>	pack ("C", 0x09),
					nargs	=>	2,
					flags_1	=>	MASK_A1M,
					flags_2	=>	MASK_A2D | MASK_A2M | MASK_A2T
				},
	mul		=>	{
					opcode	=>	pack ("C", 0x0A),
					nargs	=>	2,
					flags_1	=>	MASK_A1M,
					flags_2	=>	MASK_A2D | MASK_A2M | MASK_A2T
				},
	div		=>	{
					opcode	=>	pack ("C", 0x0B),
					nargs	=>	2,
					flags_1	=>	MASK_A1M,
					flags_2	=>	MASK_A2D | MASK_A2M | MASK_A2T
				},
	xor		=>	{
					opcode	=>	pack ("C", 0x0C),
					nargs	=>	2,
					flags_1	=>	MASK_A1M,
					flags_2	=>	MASK_A2D | MASK_A2M | MASK_A2T
				},
	push	=>	{
					opcode	=>	pack ("C", 0x0D),
					nargs	=>	1,
					flags_1	=>	MASK_A1D | MASK_A1M | MASK_A1T
				},
	pop		=>	{
					opcode	=>	pack ("C", 0x0E),
					nargs	=>	1,
					flags_1	=>	MASK_A1M | MASK_A1T
				},
	nop		=>	{
					opcode	=>	pack ("C", 0x0F),
					nargs	=>	0
				},
	and		=> 	{
					opcode	=>	pack("C", 0x10),
					nargs	=>	2,
					flags_1	=>	MASK_A1M,
					flags_2	=>	MASK_A2D | MASK_A2M | MASK_A2T
				},
	cmp		=>	{
					opcode	=>	pack("C", 0x11),
					nargs	=>	2,
					flags_1	=>	MASK_A1D | MASK_A1M | MASK_A1T,
					flags_2	=>	MASK_A2D | MASK_A2M | MASK_A2T
				}
);

my %regs = (
	rip		=>	pack ("C", 0x00),
	rsp		=>	pack ("C", 0x01),
	rbp		=>	pack ("C", 0x02),
	rer		=>	pack ("C", 0x03),
	rco		=>	pack ("C", 0x04),
	rdo		=>	pack ("C", 0x05),
	r0		=>	pack ("C", 0x06),
	r1		=>	pack ("C", 0x07),
	r2		=>	pack ("C", 0x08),
	r3		=>	pack ("C", 0x09),
	r4		=>	pack ("C", 0x0A),
	r5		=>	pack ("C", 0x0B),
	r6		=>	pack ("C", 0x0C),
	r7		=>	pack ("C", 0x0D),
	r8		=>	pack ("C", 0x0E),
	rp		=>	pack ("C", 0x0F)
);

my @instructions;
my @lines;
my %labels;
my @unr_labels;
my @data;
my $offset 	= 0;
my $count 	= 0;
my $verbose	= 0;

# Entrada / salida por defecto
my $input 		= 	*STDIN;
my $output;
my $output_f	=	"a.cvm";

# Pillar los argumentos
for (my $i = 0; $i < @ARGV; $i++) {
	if ($ARGV[$i] eq "-f") {
		open ($input, $ARGV[$i+1]);
		next;
	}
	if ($ARGV[$i] eq "-o") {
		$output_f = $ARGV[$i+1];
		next;
	}
	if ($ARGV[$i] eq "-v") {
		$verbose = 1;
		next;
	}
}
open ($output, ">:raw", $output_f);

# Dos fases: 
#	1 - Reconocimiento, offsets de las etiquetas.
#	2 - Resolver las etiquetas en las instrucciones y generar el ejecutable
# Podría hacerlo mucho mejor, evitar dos rondas, pero meh.
my $stage 		= 0;
my $unresolved 	= 0;
# Nos guardamos todo en un array
while (<$input>) {
	push (@lines, $_);
}
close ($input);
# Y a ver que hay
for ($stage = 0; $stage < 2; $stage++) {
	print "## Stage $stage\n";
	# Si se ha resuelto todo a la primera, nos ahorramos la segunda pasada
	if ($stage == 1 and $unresolved == 0) { 
		print "## All done.\n";
		next; 
	}
	$count = 0;
	$offset = 0;
	$unresolved = 0;
	@unr_labels = ();
	foreach my $line (@lines) {
		# La instrucción mínima son 2 bytes
		my $size = 2;
		$count++;
		# Limpiamos la mierda
		$line =~ s/\s+$//;
		$line =~ s/^\s+//;
		$line =~ s/\s+/ /g;
		# Los comentarios empiezan con ;
		if ($line =~ /^;/) { next; }
		# Y las lineas vacías no nos interesan
		if ($line eq "") { next; }
		# Las etiquetas (etiqueta:) van a un hash
		if ($line =~ /.*:$/) {
			if ($stage == 1) { next; }
			my $label = substr ($line, 0, -1);
			if (exists ($labels{$label})) {
				error ("redefined label '".$label."'", "", $count);
			}
			$labels{$label} = $offset;
			next;
		}
		# Vamos por partes
		my $flags = pack ("C", 0x00);
		my @parts = split (/[ ]/, $line, 2);
		my $opcode	= @parts[0];
		my $sargs	= @parts[1];
		$sargs 		=~	s/\b//g;
		# Comodamente en un array
		my @args	= split (/[ ]/, $sargs);
		if (@args[0] =~ /,$/) { chop (@args[0]); }
		# Que igual se inventan un palabro
		if (!exists ($opcodes{$opcode})) {
			error ("invalid opcode '".$opcode."'", "", $count);
		}
		# O le meten argumentos al nop...
		if (@args != $opcodes{$opcode}{nargs}) {
			error ("invalid combination", "", $count);
		}
		if (@args > 0) {
			$flags |= MASK_ARGS;
			if (@args == 2) {
				$flags |= MASK_NARGS;
			}
		}
		# Interpretamos los argumentos
		for (my $i = 0; $i < @args; $i++) {
			# El argumento es indirecto?
			if (!(@args[$i] =~ /^\[.*\]$/)) {
				$flags |= $masks[$i][MASK_AD];
			}
			else {
				@args[$i] =~ s/[\[\]]//g;
			}
			# El argumento es un registro?
			if (exists ($regs{@args[$i]})) {
				$flags |= $masks[$i][MASK_AM];
				@args[$i]	= $regs{@args[$i]};
				$size += 1;
			}
			# O simplemente un valor / etiqueta
			else {
				# Valor hexa
				if (@args[$i] =~ /^0x/) {
					# 64 bits
					if (length (@args[$i]) > 10) {
						if (length (@args[$i]) > 18) {
							error ("value exceeds 64 bits", $i, $count);
						}
						$flags |= $masks[$i][MASK_AT];
						@args[$i] = pack ("q", hex(@args[$i]));
						$size += 8;
					}
					# 32 bits
					else {
						@args[$i] = pack ("V", hex(@args[$i]));
						$size += 4;
					}
				}
				# Numero
				elsif (looks_like_number (@args[$i]) and (@args[$i] =~ m/^\d+$/)) {
					# 64 bits
					if ((@args[$i] < -2147483648) || (@args[$i] > 2147483647)) {
						if (@args[$i] < -(2**63) || @args[$i] > (2**63 - 1)) {
							error ("value exceeds 64 bits", $i, $count);
						}
						$flags |= $masks[$i][MASK_AT];
						@args[$i] = pack ("q", @args[$i]);
						$size += 8;
					}
					# 32 bits
					else {
						@args[$i] = pack ("V", @args[$i]);
						$size += 4;
					}
				}
				# Texto
				elsif (@args[$i] =~ /^".*"$/) {
					@args[$i] =~ s/"//g;
					# Cabe en 32 bits
					if (length (@args[$i]) <= 4) {
						@args[$i] = pack ("a4", @args[$i]);
						$size += 4;
					}
					# Cabe en 64 bits
					elsif (length (@args[$i]) <= 8) {
						$flags |= $masks[$i][MASK_AT];
						@args[$i] = pack ("a8", @args[$i]);
						$size += 8;
					}
					# + de 64 bits, a la zona de datos						***** TODO
					else {
						error ("argument exceeds 64 bits", $i, $count);
					}
				}
				# O una etiqueta?
				else {
					$flags |= $masks[$i][MASK_AT];
					# Ya la tenemos fichada
					if (exists ($labels{@args[$i]})) {
						@args[$i]	= pack ("q", $labels{@args[$i]});
					}
					# La tenemos que resolver más adelante
					else {
						$unresolved++;
						push (@unr_labels, @args[$i]);
					}
					$size += 8;
				} 
			}	## / valor / etiqueta
		}	## / foreach (arg)
		# Y sumamos el tamaño al offset
		$offset += $size;
		if ($verbose) {
			print $line . " (".$size.")\n";
		}
		# En el segundo ciclo metemos a la salida
		if ($stage == 1) {
			push (@instructions, $flags);
			push (@instructions, $opcodes{$opcode}{opcode});
			foreach my $a (@args) {
				push (@instructions, $a);
			}
		}
	}
	print "## $unresolved unresolved symbols\n";
	if (@unr_labels > 0) {
		foreach my $l (@unr_labels) {
			print " " . $l;
		}
		print "\n";
	}
}	## for stages

# Ahora creamos el ejecutable
$code_size = $offset;
print "## Code size: $code_size\n";
print "## Data size: $data_size\n";
print "\t\t".($data_size+$code_size)."\n";
binmode $output;
# Magic
print $output "CVM";
# code_offset & data_offset
print $output pack ("q", $code_offset).pack ("q", $data_offset);
# code_size & data_size
print $output pack ("q", $code_size).pack ("q", $data_size);
# code
foreach my $instruction (@instructions) {
	print $output $instruction;
}
# data
foreach my $a (@data) {
	print $output $a;
}
# Y a correr
close ($output);
exit (0);

sub error
{
	my $str 	= shift;
	my $narg 	= shift;
	my $line 	= shift;
	print ("Error: ".$str." at line ".$line." (".$narg.")\n");
	exit (-1);
}
