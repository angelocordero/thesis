// ignore_for_file: public_member_api_docs, sort_constructors_first
import 'dart:io';
import 'dart:math' as math;
import 'dart:math';

List<Coordinate> coordinates = [];

void main() {
  try {
    print('\n');
    print('Reading file...');
    print('\n');
    readFile();

    Coordinate start = coordinates.first;
    Coordinate end = coordinates.last;

    print('Coordinates:');
    for (Coordinate coordinate in coordinates) {
      print(coordinate);
    }

    print('\n');

    print('Getting devations...');
    List<double> deviations = [];

    for (Coordinate coordinate in coordinates) {
      double devation = getDeviation(coordinate, start, end);
      deviations.add(devation);
    }
    print('\n');

    print('Deviations: ');

    for (double deviation in deviations) {
      print(deviation);
    }

    print('\n');

    print('Calculating RMSE...');
    print('\n');
    double rmse = calculateRMSE(deviations);
    print('RMSE = $rmse');
    print('\n');
  } catch (e) {
    print(e.toString());
  }
}

void readFile() {
  File file = File('input/data.csv');

  List<String> lines = file.readAsLinesSync();

  for (String line in lines) {
    List<String> tokens = line.split(',');

    double? lat = double.tryParse(tokens[1]);
    double? long = double.tryParse(tokens[2]);

    if (lat == null || long == null) throw Exception();

    Coordinate coordinate = Coordinate(lat, long);

    coordinates.add(coordinate);
  }
}

double getDeviation(Coordinate point, Coordinate start, Coordinate end) {
  final v1 = Coordinate(end.latitude - start.latitude, end.longitude - start.longitude);
  final v2 = Coordinate(point.latitude - start.latitude, point.longitude - start.longitude);
  final l2 = v1.longitude * v1.longitude + v1.latitude * v1.latitude;
  final projection = (v2.longitude * v1.longitude + v2.latitude * v1.latitude) / l2;

  if (projection > 0 && projection < 1) {
    final uy = start.latitude + v1.latitude * projection;
    final ux = start.longitude + v1.longitude * projection;
    return getDistance(point, Coordinate(uy, ux));
  } else {
    final d1 = getDistance(point, start);
    final d2 = getDistance(point, end);
    return math.min(d1, d2);
  }
}

double getDistance(Coordinate p1, Coordinate p2) {
  final lon = p1.longitude - p2.longitude;
  final lat = p1.latitude - p2.latitude;

  double latMidpoint = (p1.latitude + p2.latitude) / 2;

  double lonMetersPerDegree = 111412.84 * cos(latMidpoint) - 93.5 * cos(3 * latMidpoint) + 0.118 * cos(5 * latMidpoint);
  double latMetersPerDegree = 111132.92 - 559.82 * cos(2 * latMidpoint) + 1.175 * cos(4 * latMidpoint) - 0.0023 * cos(6 * latMidpoint);

  double lonDistance = lon * lonMetersPerDegree;
  double latDistance = lat * latMetersPerDegree;

  return math.sqrt((lonDistance * lonDistance) + (latDistance * latDistance));
}

class Coordinate {
  double latitude;
  double longitude;

  Coordinate(
    this.latitude,
    this.longitude,
  );

  @override
  String toString() => 'Coordinate(latitude: $latitude, longitude: $longitude)';
}

double calculateRMSE(List<double> deviations) {
  double squaredErrors = 0.0;

  for (int i = 0; i < deviations.length; i++) {
    double deviation = deviations[i];
    squaredErrors += deviation * deviation;
  }

  return math.sqrt(squaredErrors / deviations.length);
}
