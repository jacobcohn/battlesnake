# Deployment

This project can be deployed as a Docker container. The current deployment target is AWS ECS Fargate with images stored in Amazon ECR.

## Prerequisites

- Docker with Buildx enabled
- AWS CLI installed and authenticated
- An existing ECR repository
- An existing ECS cluster and service

## Configuration

Create a local `.env` file from the example:

```bash
cp .env.example .env
```

Fill in the deployment values:

```bash
AWS_REGION=us-east-1
AWS_ACCOUNT_ID=<aws-account-id>
ECR_IMAGE=<aws-account-id>.dkr.ecr.<region>.amazonaws.com/battlesnake:latest
ECS_CLUSTER=battlesnake-cluster
ECS_SERVICE=battlesnake-service
```

Then load the values:

```bash
source .env
```

## Authenticate Docker with ECR

```bash
aws ecr get-login-password --region $AWS_REGION \
  | docker login --username AWS --password-stdin \
    $AWS_ACCOUNT_ID.dkr.ecr.$AWS_REGION.amazonaws.com
```

## Build and Push the Image

Build and push an ARM64 image for Fargate:

```bash
docker buildx build \
  --platform linux/arm64 \
  -t $ECR_IMAGE \
  --push .
```

## Redeploy the ECS Service

Force a new deployment so ECS pulls the latest image:

```bash
aws ecs update-service \
  --cluster $ECS_CLUSTER \
  --service $ECS_SERVICE \
  --force-new-deployment
```

## Notes

- The Fargate public IP may change after redeployment.
- The container exposes port `8080`.
- Make sure the ECS task definition, service, and security group route traffic to the container's `8080` port.
