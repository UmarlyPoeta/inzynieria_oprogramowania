# =========================
# > Build Stage
# =========================

FROM node:22-alpine AS build        # Use Node.js 22 on Alpine Linux
WORKDIR /app                        # Set working directory inside the container
COPY package*.json ./               # Copy package.json and package-lock.json
RUN npm install                     # Install project dependencies
COPY . .                            # Copy the rest of the source code
RUN npm run build                   # Build the React + Vite project (outputs /dist)

# =========================
# > Production Stage
# =========================

FROM nginx:alpine                   # Use lightweight Nginx image for production
COPY --from=build /app/dist /usr/share/nginx/html  # Copy built frontend to Nginx html folder
EXPOSE 80                           # Expose port 80 for HTTP traffic
CMD ["nginx", "-g", "daemon off;"]  # Run Nginx in foreground to keep container running
